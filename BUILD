package(default_visibility = ["//visibility:public"])

load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_binary",
    "envoy_cc_library",
    "envoy_cc_test",
)

envoy_cc_binary(
    name = "envoy",
    repository = "@envoy",
    deps = [
        ":inner_tls_inspector_config",
        "@envoy//source/exe:envoy_main_entry_lib",
    ],
)

envoy_cc_library(
    name = "inner_tls_inspector_lib",
    srcs = ["inner_tls_inspector.cc"],
    hdrs = ["inner_tls_inspector.h"],
    repository = "@envoy",
    external_deps = ["ssl"],
    deps = [
        "@envoy//include/envoy/buffer:buffer_interface",
        "@envoy//include/envoy/network:connection_interface",
        "@envoy//include/envoy/network:filter_interface",
        "@envoy//source/common/common:assert_lib",
        "@envoy//source/common/common:logger_lib",
    ],
)

envoy_cc_library(
    name = "inner_tls_inspector_config",
    srcs = ["inner_tls_inspector_config.cc"],
    repository = "@envoy",
    deps = [
        ":inner_tls_inspector_lib",
        "@envoy//include/envoy/network:filter_interface",
        "@envoy//include/envoy/registry:registry",
        "@envoy//include/envoy/server:filter_config_interface",
    ],
)