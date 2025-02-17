//! Support for a calling of an imported function.

use crate::trampoline::StoreInstanceHandle;
use crate::Store;
use anyhow::Result;
use std::any::Any;
use std::sync::Arc;
use wasmtime_environ::entity::PrimaryMap;
use wasmtime_environ::wasm::DefinedFuncIndex;
use wasmtime_environ::Module;
use wasmtime_runtime::{
    Imports, InstanceHandle, StackMapRegistry, VMExternRefActivationsTable, VMFunctionBody,
    VMFunctionImport,
};

pub(crate) fn create_handle(
    module: Module,
    store: &Store,
    finished_functions: PrimaryMap<DefinedFuncIndex, *mut [VMFunctionBody]>,
    state: Box<dyn Any>,
    func_imports: &[VMFunctionImport],
) -> Result<StoreInstanceHandle> {
    let mut imports = Imports::default();
    imports.functions = func_imports;
    let module = Arc::new(module);
    let module2 = module.clone();

    unsafe {
        let handle = InstanceHandle::new(
            module,
            Arc::new(()),
            &finished_functions,
            imports,
            store.memory_creator(),
            &store.lookup_shared_signature(&module2),
            state,
            store.interrupts(),
            store.externref_activations_table() as *const VMExternRefActivationsTable as *mut _,
            store.stack_map_registry() as *const StackMapRegistry as *mut _,
        )?;
        Ok(store.add_instance(handle))
    }
}
