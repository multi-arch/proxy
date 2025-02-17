// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>  // for uintptr_t

#include <string>
#include <tuple>    // for std::tie
#include <utility>  // for std::swap

#include "base/memory/checked_ptr.h"

class SomeClass {};
class DerivedClass : public SomeClass {};

struct MyStruct {
  CheckedPtr<SomeClass> ptr;
  CheckedPtr<SomeClass> ptr2;
  CheckedPtr<const SomeClass> const_ptr;
  int (*func_ptr_field)();
  CheckedPtr<const char> const_char_ptr;
};

namespace auto_tests {

MyStruct* GetMyStruct() {
  return nullptr;
}

SomeClass* GetSomeClass() {
  return nullptr;
}

SomeClass* ConvertSomeClassToSomeClass(SomeClass* some_class) {
  return some_class;
}

void foo() {
  MyStruct my_struct;

  // After the rewrite |my_struct.ptr_field| is no longer a pointer,
  // so |auto*| won't work.  We fix this up, by appending |.get()|.
  // Expected rewrite: auto* ptr_var = my_struct.ptr.get();
  auto* ptr_var = my_struct.ptr.get();

  // Tests for other kinds of initialization.
  // Expected rewrite: |.get()| should be appended in both cases below.
  auto* init_test1(my_struct.ptr.get());
  auto* init_test2{my_struct.ptr.get()};

  // Test for handling of the |const| qualifier.
  // Expected rewrite: const auto* ptr_var = my_struct.ptr.get();
  const auto* const_ptr_var = my_struct.ptr.get();

  // More complicated initialization expression, but the |ptr_field| struct
  // member dereference is still the top/last expression here.
  // Expected rewrite: ...->ptr.get()
  auto* complicated_var = GetMyStruct()->ptr.get();

  // The test below covers:
  // 1. Two variables with single |auto|,
  // 2. Tricky placement of |*| (next to the variable name).
  // Expected rewrite: ...ptr.get()... (twice in the 2nd example).
  auto *ptr_var1 = my_struct.ptr.get(), *ptr_var2 = GetSomeClass();
  auto *ptr_var3 = my_struct.ptr.get(), *ptr_var4 = my_struct.ptr.get();
  auto *ptr_var5 = GetSomeClass(), *ptr_var6 = my_struct.ptr.get();

  // Test for the case where
  // 1. The resulting type is the same as in the |ptr_var| and |complicated_var|
  //    examples
  // 2. Deep in the initialization expression there is a member dereference
  //    of |ptr_field|
  // but
  // 3. The final/top-level initialization expression doesn't dereference
  //    |ptr_field|.
  // No rewrite expected.
  auto* not_affected_field_var = ConvertSomeClassToSomeClass(my_struct.ptr);

  // Test for pointer |auto| assigned from non-CheckedPtr-elligible field.
  // No rewrite expected.
  auto* func_ptr_var = my_struct.func_ptr_field;

  // Test for non-pointer |auto| assigned from CheckedPtr-elligible field.
  // No rewrite expected.
  auto non_pointer_auto_var = my_struct.ptr;

  // Test for non-auto pointer.
  // No rewrite expected.
  SomeClass* non_auto_ptr_var = my_struct.ptr;
}

}  // namespace auto_tests

namespace printf_tests {

int ConvertSomeClassToInt(SomeClass* some_class) {
  return 123;
}

void MyPrintf(const char* fmt, ...) {}

void foo() {
  MyStruct s;

  // Expected rewrite: MyPrintf("%p", s.ptr.get());
  MyPrintf("%p", s.ptr.get());

  // Test - all arguments are rewritten.
  // Expected rewrite: MyPrintf("%p, %p", s.ptr.get(), s.ptr2.get());
  MyPrintf("%p, %p", s.ptr.get(), s.ptr2.get());

  // Test - only |s.ptr|-style arguments are rewritten.
  // Expected rewrite: MyPrintf("%d, %p", 123, s.ptr.get());
  MyPrintf("%d, %p", 123, s.ptr.get());

  // Test - |s.ptr| is deeply nested.
  // No rewrite expected.
  MyPrintf("%d", ConvertSomeClassToInt(s.ptr));
}

}  // namespace printf_tests

namespace cast_tests {

void foo() {
  MyStruct my_struct;

  // To get |const_cast<...>(...)| to compile after the rewrite we
  // need to rewrite the casted expression.
  // Expected rewrite: const_cast<SomeClass*>(my_struct.const_ptr.get());
  SomeClass* v = const_cast<SomeClass*>(my_struct.const_ptr.get());
  // Expected rewrite: const_cast<const SomeClass*>(my_struct.ptr.get());
  const SomeClass* v2 = const_cast<const SomeClass*>(my_struct.ptr.get());

  // To get |reinterpret_cast<uintptr_t>(...)| to compile after the rewrite we
  // need to rewrite the casted expression.
  // Expected rewrite: reinterpret_cast<uintptr_t>(my_struct.ptr.get());
  uintptr_t u = reinterpret_cast<uintptr_t>(my_struct.ptr.get());

  // There is no need to append |.get()| inside static_cast - unlike the
  // const_cast and reinterpret_cast examples above, static_cast will compile
  // just fine.
  DerivedClass* d = static_cast<DerivedClass*>(my_struct.ptr);
  void* void_var = static_cast<void*>(my_struct.ptr);
}

}  // namespace cast_tests

namespace ternary_operator_tests {

void foo(int x) {
  MyStruct my_struct;
  SomeClass* other_ptr = nullptr;

  // To avoid the following error type:
  //     conditional expression is ambiguous; 'const CheckedPtr<SomeClass>'
  //     can be converted to 'SomeClass *' and vice versa
  // we need to append |.get()| to |my_struct.ptr| below.
  //
  // Expected rewrite: ... my_struct.ptr.get() ...
  SomeClass* v = (x > 123) ? my_struct.ptr.get() : other_ptr;

  // Rewrite in the other position.
  // Expected rewrite: ... my_struct.ptr.get() ...
  SomeClass* v2 = (x > 456) ? other_ptr : my_struct.ptr.get();

  // No rewrite is needed for the first, conditional argument.
  // No rewrite expected.
  int v3 = my_struct.ptr ? 123 : 456;

  // Test for 1st and 2nd arg.  Only 2nd arg should be rewritten.
  SomeClass* v4 = my_struct.ptr ? my_struct.ptr.get() : other_ptr;
}

}  // namespace ternary_operator_tests

namespace templated_functions {

template <typename T>
void AffectedFunction(T* t) {}

template <typename T>
void TemplatedFunction_NonTemplatedParam(SomeClass* arg, T t) {}

template <typename T>
class MyTemplate {
 public:
  template <typename U>
  MyTemplate(U* u) {}

  void AffectedMethod(T* t) {}
};

// We also want to append |.get()| for |T| parameters (i.e. not just for |T*|
// parameters).
//
// One motivating example is ActivityLogDatabasePolicy::ScheduleAndForget which
// passes its argument to base::Unretained.
//
// Another motivating example, is the following pattern from
// //components/variations/service/ui_string_overrider.cc where the type of the
// 2 arguments needs to be kept consistent:
//     const uint32_t* end = ptr_field_ + num_resources_;
//     const uint32_t* element = std::lower_bound(ptr_field_, end, hash);
template <typename T>
void AffectedNonPointerFunction(T t) {}

// AffectedFunctionWithDeepT mimics ConvertPPResourceArrayToObjects from
// //ppapi/cpp/array_output.h
template <typename T>
void AffectedFunctionWithDeepT(MyTemplate<T>* blah) {}

// StructWithPointerToTemplate is used to test AffectedFunctionWithDeepT.
// StructWithPointerToTemplate mimics ResourceArrayOutputAdapter<T>
// (and its |output_| field that will be converted to a CheckedPtr)
// from //ppapi/cpp/array_output.h
template <typename T>
struct StructWithPointerToTemplate {
  CheckedPtr<MyTemplate<T>> ptr_to_template;
};

void foo() {
  MyStruct my_struct;

  // Expected rewrite - appending: .get()
  AffectedFunction(my_struct.ptr.get());

  // Expected rewrite - appending: .get()
  MyTemplate<SomeClass> mt(my_struct.ptr.get());
  // Expected rewrite - appending: .get()
  mt.AffectedMethod(my_struct.ptr.get());

  // No rewrite expected.
  TemplatedFunction_NonTemplatedParam(my_struct.ptr, 123);

  // Expected rewrite - appending: .get()
  AffectedNonPointerFunction(my_struct.ptr.get());

  // Expected rewrite - appending: .get()
  StructWithPointerToTemplate<SomeClass> swptt;
  AffectedFunctionWithDeepT(swptt.ptr_to_template.get());

  // No rewrite expected - T& parameter.
  std::swap(my_struct.ptr, my_struct.ptr2);
  std::tie(my_struct.ptr, my_struct.ptr2) = std::make_pair(nullptr, nullptr);
}

}  // namespace templated_functions

namespace implicit_constructors {

// Based on //base/strings/string_piece_forward.h:
template <typename STRING_TYPE>
class BasicStringPiece;
typedef BasicStringPiece<std::string> StringPiece;
// Based on //base/strings/string_piece.h:
template <typename STRING_TYPE>
class BasicStringPiece {
 public:
  constexpr BasicStringPiece(const char* str) {}
};
// Test case:
void FunctionTakingBasicStringPiece(StringPiece arg) {}

class ClassWithImplicitConstructor {
 public:
  ClassWithImplicitConstructor(SomeClass* blah) {}
};
void FunctionTakingArgWithImplicitConstructor(
    ClassWithImplicitConstructor arg) {}

void foo() {
  MyStruct my_struct;

  // Expected rewrite - appending: .get().  This avoids the following error:
  // error: no matching function for call to 'FunctionTakingBasicStringPiece'
  // note: candidate function not viable: no known conversion from
  // 'base::CheckedPtr<const char>' to 'templated_functions::StringPiece' (aka
  // 'BasicStringPiece<basic_string<char, char_traits<char>, allocator<char>>>')
  // for 1st argument
  FunctionTakingBasicStringPiece(my_struct.const_char_ptr.get());

  // Expected rewrite - appending: .get().  This is the same scenario as with
  // StringPiece above (except that no templates are present here).
  FunctionTakingArgWithImplicitConstructor(my_struct.ptr.get());
}

}  // namespace implicit_constructors

namespace affected_implicit_template_specialization {

template <typename T, typename T2>
struct MyTemplate {
  CheckedPtr<T> t_ptr;
  CheckedPtr<T2> t2_ptr;

  struct NestedStruct {
    CheckedPtr<SomeClass> nested_ptr_field;
    CheckedPtr<T> nested_t_ptr_field;
  };
  NestedStruct nested_struct_field;
};

template <typename T3>
struct MyTemplate<SomeClass, T3> {
  CheckedPtr<SomeClass> some_ptr;
  CheckedPtr<T3> t3_ptr;
};

// The example that forces explicit |isAnonymousStructOrUnion| checks in
// the implementation of GetExplicitDecl.  The example is based on
// buildtools/third_party/libc++/trunk/include/string.
template <typename T>
struct MyStringTemplate {
  struct NestedStruct {
    union {
      long l;
      short s;
      CheckedPtr<T> t_ptr;
      CheckedPtr<int> i_ptr;
    };  // Unnamed / anonymous union *field*.

    struct {
      long l2;
      short s2;
      CheckedPtr<T> t_ptr2;
      CheckedPtr<int> i_ptr2;
    };  // Unnamed / anonymous struct *field*.
  };
  NestedStruct s;
};

void MyPrintf(const char* fmt, ...) {}

void foo() {
  // |s.t_ptr| comes from implicit template specialization (which needs to be
  // skipped for rewriting, but should be included for appending |.get()|).
  //
  // Expected rewrite: MyPrintf("%p", s.t_ptr.get());
  MyTemplate<int, int> s;
  MyPrintf("%p", s.t_ptr.get());

  // |s.some_ptr| and |s.t2_ptr| come from implicit template specialization or a
  // partial template specialization.
  //
  // Expected rewrite: MyPrintf("%p", s.some_ptr.get(), s.t3_ptr.get());
  MyTemplate<SomeClass, int> s2;
  MyPrintf("%p %p", s2.some_ptr.get(), s2.t3_ptr.get());

  // Nested structs require extra care when trying to look up the non-implicit
  // field definition.  Expected rewrite: adding |.get()| suffix.
  MyPrintf("%p", s.nested_struct_field.nested_ptr_field.get());
  MyPrintf("%p", s.nested_struct_field.nested_t_ptr_field.get());

  // Lines below are added mainly to Force implicit specialization of
  // MyStringTemplate (to force explicit |isAnonymousStructOrUnion| checks in
  // the rewriter).  Still, the expected rewrite is: appending |.get()| to the
  // printf arg.
  MyStringTemplate<void> mst;
  MyPrintf("%p %p", mst.s.t_ptr.get(), mst.s.t_ptr2.get());
}

}  // namespace affected_implicit_template_specialization

// The test scenario below is based on an example encountered in
// //cc/layers/picture_layer_impl_unittest.cc:
//   auto* shared_quad_state = render_pass->quad_list.begin()->shared_quad_state
// In this example, the AST looks like this:
//  `-DeclStmt
//    `-VarDecl shared_quad_state 'const SharedQuadState *' cinit
//      `-ExprWithCleanups 'const SharedQuadState *'
//        `-ImplicitCastExpr 'const SharedQuadState *' <LValueToRValue>
//          `-MemberExpr 'const SharedQuadState *const' lvalue ->shared...state
//            `-.....
// The rewriter needs to ignore the implicit ExprWithCleanups and
// ImplicitCastExpr nodes in order to find the MemberExpr.  If this is
// implemented incorrectly, then the rewriter won't append |.get()| to fix the
// |auto*| initialization.
namespace more_implicit_ast_nodes_trouble {

template <class BaseElementType>
struct ListContainer {
  struct ConstIterator {
    const BaseElementType* operator->() const { return nullptr; }
  };

  ConstIterator begin() const { return ConstIterator(); }
};

class SharedQuadState;

struct DrawQuad {
  CheckedPtr<const SharedQuadState> shared_quad_state;
};

struct RenderPass {
  using QuadList = ListContainer<DrawQuad>;
  QuadList quad_list;
};

void foo() {
  RenderPass* render_pass = nullptr;
  auto* shared_quad_state =
      render_pass->quad_list.begin()->shared_quad_state.get();
}

}  // namespace more_implicit_ast_nodes_trouble
