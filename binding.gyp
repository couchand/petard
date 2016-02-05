{
  "targets": [
    {
      "target_name": "petard",
      "sources": [
        "src/utils/llvm_utils.cpp",
        "src/petard/type.cpp",
        "src/petard/value.cpp",
        "src/petard/block_builder.cpp",
        "src/petard/function_builder.cpp",
        "src/petard/switch_builder.cpp",
        "src/petard/code_unit.cpp",
        "src/node/type_wrapper.cpp",
        "src/node/value_wrapper.cpp",
        "src/node/builder_wrapper.cpp",
        "src/node/function_builder_wrapper.cpp",
        "src/node/switch_builder_wrapper.cpp",
        "src/node/code_unit_wrapper.cpp",
        "src/node/api.cpp"
      ],
      "defines": [
        "__STDC_LIMIT_MACROS=1",
        "__STDC_CONSTANT_MACROS=1"
      ],
      "include_dirs": [
        "include/utils",
        "include/petard",
        "include/node",
        "<!(node -e \"require(\'nan\')\")",
        "<!($LLVM_CONFIG --includedir)"
      ],
      "libraries": [
        "<!@($LLVM_CONFIG --ldflags)",
        "<!@($LLVM_CONFIG --libs core native support bitwriter)"
      ]
    }
  ]
}
