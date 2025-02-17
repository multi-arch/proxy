# This package contains implementation of expression evaluator
# internals.
package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

exports_files(["LICENSE"])

cc_library(
    name = "evaluator_core",
    srcs = [
        "evaluator_core.cc",
    ],
    hdrs = [
        "evaluator_core.h",
    ],
    deps = [
        ":attribute_trail",
        ":attribute_utility",
        "//base:status_macros",
        "//eval/public:activation",
        "//eval/public:cel_attribute",
        "//eval/public:cel_expression",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "expression_step_base",
    hdrs = [
        "expression_step_base.h",
    ],
    deps = [
        ":evaluator_core",
        "//eval/public:activation",
        "//eval/public:cel_expression",
        "//eval/public:cel_value",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "const_value_step",
    srcs = [
        "const_value_step.cc",
    ],
    hdrs = [
        "const_value_step.h",
    ],
    deps = [
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public:activation",
        "//eval/public:cel_expression",
        "//eval/public:cel_value",
        "//eval/public/structs:cel_proto_wrapper",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "container_access_step",
    srcs = [
        "container_access_step.cc",
    ],
    hdrs = [
        "container_access_step.h",
    ],
    deps = [
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public:activation",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "ident_step",
    srcs = [
        "ident_step.cc",
    ],
    hdrs = [
        "ident_step.h",
    ],
    deps = [
        ":attribute_trail",
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public:activation",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "function_step",
    srcs = [
        "function_step.cc",
    ],
    hdrs = [
        "function_step.h",
    ],
    deps = [
        ":attribute_trail",
        ":evaluator_core",
        ":expression_build_warning",
        ":expression_step_base",
        "//base:status_macros",
        "//eval/public:activation",
        "//eval/public:cel_builtins",
        "//eval/public:cel_function",
        "//eval/public:cel_function_provider",
        "//eval/public:cel_function_registry",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "//eval/public:unknown_function_result_set",
        "//eval/public:unknown_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "select_step",
    srcs = [
        "select_step.cc",
    ],
    hdrs = [
        "select_step.h",
    ],
    deps = [
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public:activation",
        "//eval/public:cel_value",
        "//eval/public/containers:field_access",
        "//eval/public/containers:field_backed_list_impl",
        "//eval/public/containers:field_backed_map_impl",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "create_list_step",
    srcs = [
        "create_list_step.cc",
    ],
    hdrs = [
        "create_list_step.h",
    ],
    deps = [
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public:activation",
        "//eval/public:cel_value",
        "//eval/public/containers:container_backed_list_impl",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
    ],
)

cc_library(
    name = "create_struct_step",
    srcs = [
        "create_struct_step.cc",
    ],
    hdrs = [
        "create_struct_step.h",
    ],
    deps = [
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public/containers:container_backed_map_impl",
        "//eval/public/containers:field_access",
        "//eval/public/structs:cel_proto_wrapper",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
    ],
)

cc_library(
    name = "jump_step",
    srcs = [
        "jump_step.cc",
    ],
    hdrs = [
        "jump_step.h",
    ],
    deps = [
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public:activation",
        "//eval/public:cel_value",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
    ],
)

cc_library(
    name = "logic_step",
    srcs = [
        "logic_step.cc",
    ],
    hdrs = [
        "logic_step.h",
    ],
    deps = [
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public:activation",
        "//eval/public:cel_builtins",
        "//eval/public:cel_function",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
    ],
)

cc_library(
    name = "comprehension_step",
    srcs = [
        "comprehension_step.cc",
    ],
    hdrs = [
        "comprehension_step.h",
    ],
    deps = [
        ":attribute_trail",
        ":evaluator_core",
        ":expression_step_base",
        "//base:status_macros",
        "//eval/public:activation",
        "//eval/public:cel_attribute",
        "//eval/public:cel_function",
        "//eval/public:cel_value",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
    ],
)

cc_test(
    name = "comprehension_step_test",
    size = "small",
    srcs = [
        "comprehension_step_test.cc",
    ],
    deps = [
        ":comprehension_step",
        ":evaluator_core",
        ":ident_step",
        "//base:status_macros",
        "//eval/public:cel_attribute",
        "//eval/public:cel_options",
        "//eval/public:cel_value",
        "//eval/public/structs:cel_proto_wrapper",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_test(
    name = "evaluator_core_test",
    size = "small",
    srcs = [
        "evaluator_core_test.cc",
    ],
    deps = [
        ":attribute_trail",
        ":evaluator_core",
        "//base:status_macros",
        "//eval/compiler:flat_expr_builder",
        "//eval/public:builtin_func_registrar",
        "//eval/public:cel_attribute",
        "//eval/public:cel_value",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "const_value_step_test",
    size = "small",
    srcs = [
        "const_value_step_test.cc",
    ],
    deps = [
        ":const_value_step",
        ":evaluator_core",
        "//base:status_macros",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "container_access_step_test",
    size = "small",
    srcs = [
        "container_access_step_test.cc",
    ],
    deps = [
        ":container_access_step",
        ":ident_step",
        "//base:status_macros",
        "//eval/public:cel_attribute",
        "//eval/public:cel_builtins",
        "//eval/public:cel_value",
        "//eval/public/containers:container_backed_list_impl",
        "//eval/public/containers:container_backed_map_impl",
        "//eval/public/structs:cel_proto_wrapper",
        "@com_google_googletest//:gtest_main",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_test(
    name = "ident_step_test",
    size = "small",
    srcs = [
        "ident_step_test.cc",
    ],
    deps = [
        ":evaluator_core",
        ":ident_step",
        "//base:status_macros",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "function_step_test",
    size = "small",
    srcs = [
        "function_step_test.cc",
    ],
    deps = [
        ":evaluator_core",
        ":expression_build_warning",
        ":function_step",
        ":ident_step",
        "//base:status_macros",
        "//eval/public:cel_attribute",
        "//eval/public:cel_function",
        "//eval/public:cel_options",
        "//eval/public:cel_value",
        "//eval/public:unknown_function_result_set",
        "//eval/public/structs:cel_proto_wrapper",
        "//eval/testutil:test_message_cc_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "logic_step_test",
    size = "small",
    srcs = [
        "logic_step_test.cc",
    ],
    deps = [
        ":ident_step",
        ":logic_step",
        "//base:status_macros",
        "//eval/public:unknown_attribute_set",
        "//eval/public:unknown_set",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "select_step_test",
    size = "small",
    srcs = [
        "select_step_test.cc",
    ],
    deps = [
        ":ident_step",
        ":select_step",
        "//base:status_macros",
        "//eval/public:cel_attribute",
        "//eval/public:unknown_attribute_set",
        "//eval/public/containers:container_backed_map_impl",
        "//eval/public/structs:cel_proto_wrapper",
        "//eval/testutil:test_message_cc_proto",
        "//testutil:util",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "create_list_step_test",
    size = "small",
    srcs = [
        "create_list_step_test.cc",
    ],
    deps = [
        ":const_value_step",
        ":create_list_step",
        ":ident_step",
        "//base:status_macros",
        "//eval/public:activation",
        "//eval/public:cel_attribute",
        "//eval/public:unknown_attribute_set",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "create_struct_step_test",
    size = "small",
    srcs = [
        "create_struct_step_test.cc",
    ],
    deps = [
        ":create_struct_step",
        ":ident_step",
        "//base:status_macros",
        "//eval/public/containers:container_backed_list_impl",
        "//eval/public/containers:container_backed_map_impl",
        "//eval/public/structs:cel_proto_wrapper",
        "//eval/testutil:test_message_cc_proto",
        "//testutil:util",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "expression_build_warning",
    srcs = [
        "expression_build_warning.cc",
    ],
    hdrs = [
        "expression_build_warning.h",
    ],
    deps = [
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "expression_build_warning_test",
    size = "small",
    srcs = [
        "expression_build_warning_test.cc",
    ],
    deps = [
        ":expression_build_warning",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "attribute_trail",
    srcs = ["attribute_trail.cc"],
    hdrs = ["attribute_trail.h"],
    deps = [
        "//eval/public:activation",
        "//eval/public:cel_attribute",
        "//eval/public:cel_expression",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:optional",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_test(
    name = "attribute_trail_test",
    size = "small",
    srcs = [
        "attribute_trail_test.cc",
    ],
    deps = [
        ":attribute_trail",
        "//eval/public:cel_attribute",
        "//eval/public:cel_value",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "attribute_utility",
    srcs = ["attribute_utility.cc"],
    hdrs = ["attribute_utility.h"],
    deps = [
        ":attribute_trail",
        "//eval/public:activation",
        "//eval/public:cel_attribute",
        "//eval/public:cel_expression",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "//eval/public:unknown_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_test(
    name = "attribute_utility_test",
    size = "small",
    srcs = [
        "attribute_utility_test.cc",
    ],
    deps = [
        ":attribute_utility",
        "//eval/public:cel_attribute",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "//eval/public:unknown_set",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ternary_step",
    srcs = [
        "ternary_step.cc",
    ],
    hdrs = [
        "ternary_step.h",
    ],
    deps = [
        ":evaluator_core",
        ":expression_step_base",
        "//eval/public:activation",
        "//eval/public:cel_builtins",
        "//eval/public:cel_function",
        "//eval/public:cel_value",
        "//eval/public:unknown_attribute_set",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
    ],
)

cc_test(
    name = "ternary_step_test",
    size = "small",
    srcs = [
        "ternary_step_test.cc",
    ],
    deps = [
        ":ident_step",
        ":ternary_step",
        "//base:status_macros",
        "//eval/public:unknown_attribute_set",
        "//eval/public:unknown_set",
        "@com_google_googletest//:gtest_main",
    ],
)
