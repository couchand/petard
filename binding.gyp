{
  "targets": [
    {
      "target_name": "codegen",
      "sources": ["src/api.cpp", "src/codegen.cpp", "src/types.cpp", "src/function_builder.cpp", "src/value.cpp"],
      "defines": [
        "__STDC_LIMIT_MACROS=1",
        "__STDC_CONSTANT_MACROS=1"
      ],
      "include_dirs": ["include", "<!(node -e \"require(\'nan\')\")", "<!($LLVM_CONFIG --includedir)"],
      "libraries": [
        "<!@($LLVM_CONFIG --ldflags)",
        "<!@($LLVM_CONFIG --libs core native support bitwriter)"
      ]
    }
  ]
}
