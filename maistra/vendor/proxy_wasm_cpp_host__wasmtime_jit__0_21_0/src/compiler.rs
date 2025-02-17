//! JIT compilation.

use crate::instantiate::SetupError;
use crate::object::{build_object, ObjectUnwindInfo};
use object::write::Object;
use std::hash::{Hash, Hasher};
use std::mem;
use wasmparser::WasmFeatures;
use wasmtime_debug::{emit_dwarf, DwarfSection};
use wasmtime_environ::entity::EntityRef;
use wasmtime_environ::isa::{TargetFrontendConfig, TargetIsa};
use wasmtime_environ::wasm::{DefinedMemoryIndex, MemoryIndex};
use wasmtime_environ::{
    CompiledFunctions, Compiler as EnvCompiler, DebugInfoData, Module, ModuleMemoryOffset,
    ModuleTranslation, Tunables, VMOffsets,
};

/// Select which kind of compilation to use.
#[derive(Copy, Clone, Debug, Hash)]
pub enum CompilationStrategy {
    /// Let Wasmtime pick the strategy.
    Auto,

    /// Compile all functions with Cranelift.
    Cranelift,

    /// Compile all functions with Lightbeam.
    #[cfg(feature = "lightbeam")]
    Lightbeam,
}

/// A WebAssembly code JIT compiler.
///
/// A `Compiler` instance owns the executable memory that it allocates.
///
/// TODO: Evolve this to support streaming rather than requiring a `&[u8]`
/// containing a whole wasm module at once.
///
/// TODO: Consider using cranelift-module.
pub struct Compiler {
    isa: Box<dyn TargetIsa>,
    compiler: Box<dyn EnvCompiler>,
    strategy: CompilationStrategy,
    tunables: Tunables,
    features: WasmFeatures,
}

impl Compiler {
    /// Construct a new `Compiler`.
    pub fn new(
        isa: Box<dyn TargetIsa>,
        strategy: CompilationStrategy,
        tunables: Tunables,
        features: WasmFeatures,
    ) -> Self {
        Self {
            isa,
            strategy,
            compiler: match strategy {
                CompilationStrategy::Auto | CompilationStrategy::Cranelift => {
                    Box::new(wasmtime_cranelift::Cranelift::default())
                }
                #[cfg(feature = "lightbeam")]
                CompilationStrategy::Lightbeam => Box::new(wasmtime_lightbeam::Lightbeam),
            },
            tunables,
            features,
        }
    }
}

fn _assert_compiler_send_sync() {
    fn _assert<T: Send + Sync>() {}
    _assert::<Compiler>();
}

fn transform_dwarf_data(
    isa: &dyn TargetIsa,
    module: &Module,
    debug_data: &DebugInfoData,
    funcs: &CompiledFunctions,
) -> Result<Vec<DwarfSection>, SetupError> {
    let target_config = isa.frontend_config();
    let ofs = VMOffsets::new(target_config.pointer_bytes(), &module);

    let memory_offset = if ofs.num_imported_memories > 0 {
        ModuleMemoryOffset::Imported(ofs.vmctx_vmmemory_import(MemoryIndex::new(0)))
    } else if ofs.num_defined_memories > 0 {
        ModuleMemoryOffset::Defined(ofs.vmctx_vmmemory_definition_base(DefinedMemoryIndex::new(0)))
    } else {
        ModuleMemoryOffset::None
    };
    emit_dwarf(isa, debug_data, funcs, &memory_offset).map_err(SetupError::DebugInfo)
}

#[allow(missing_docs)]
pub struct Compilation {
    pub obj: Object,
    pub unwind_info: Vec<ObjectUnwindInfo>,
    pub funcs: CompiledFunctions,
}

impl Compiler {
    /// Return the isa.
    pub fn isa(&self) -> &dyn TargetIsa {
        self.isa.as_ref()
    }

    /// Return the target's frontend configuration settings.
    pub fn frontend_config(&self) -> TargetFrontendConfig {
        self.isa.frontend_config()
    }

    /// Return the tunables in use by this engine.
    pub fn tunables(&self) -> &Tunables {
        &self.tunables
    }

    /// Return the enabled wasm features.
    pub fn features(&self) -> &WasmFeatures {
        &self.features
    }

    /// Compile the given function bodies.
    pub fn compile<'data>(
        &self,
        translation: &mut ModuleTranslation,
    ) -> Result<Compilation, SetupError> {
        let functions = mem::take(&mut translation.function_body_inputs);
        cfg_if::cfg_if! {
            if #[cfg(feature = "parallel-compilation")] {
                use rayon::prelude::*;
                let iter = functions
                    .into_iter()
                    .collect::<Vec<_>>()
                    .into_par_iter();
            } else {
                let iter = functions.into_iter();
            }
        }
        let funcs = iter
            .map(|(index, func)| {
                self.compiler
                    .compile_function(translation, index, func, &*self.isa)
            })
            .collect::<Result<Vec<_>, _>>()?
            .into_iter()
            .collect::<CompiledFunctions>();

        let dwarf_sections = if translation.debuginfo.is_some() && !funcs.is_empty() {
            transform_dwarf_data(
                &*self.isa,
                &translation.module,
                translation.debuginfo.as_ref().unwrap(),
                &funcs,
            )?
        } else {
            vec![]
        };

        let (obj, unwind_info) = build_object(&*self.isa, &translation, &funcs, dwarf_sections)?;

        Ok(Compilation {
            obj,
            unwind_info,
            funcs,
        })
    }
}

impl Hash for Compiler {
    fn hash<H: Hasher>(&self, hasher: &mut H) {
        let Compiler {
            strategy,
            compiler: _,
            isa,
            tunables,
            features,
        } = self;

        // Hash compiler's flags: compilation strategy, isa, frontend config,
        // misc tunables.
        strategy.hash(hasher);
        isa.triple().hash(hasher);
        features.hash(hasher);
        // TODO: if this `to_string()` is too expensive then we should upstream
        // a native hashing ability of flags into cranelift itself, but
        // compilation and/or cache loading is relatively expensive so seems
        // unlikely.
        isa.flags().to_string().hash(hasher);
        isa.frontend_config().hash(hasher);
        tunables.hash(hasher);

        // TODO: ... and should we hash anything else? There's a lot of stuff in
        // `TargetIsa`, like registers/encodings/etc. Should we be hashing that
        // too? It seems like wasmtime doesn't configure it too too much, but
        // this may become an issue at some point.
    }
}
