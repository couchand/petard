{
  "targets": [
    {
      "target_name": "codegen",
      "sources": ["src/api.cpp", "src/codegen.cpp", "src/types.cpp", "src/function_builder.cpp", "src/value.cpp"],
      "defines": [
        "__STDC_LIMIT_MACROS=1",
        "__STDC_CONSTANT_MACROS=1"
      ],
      "include_dirs": ["include", "<!(node -e \"require(\'nan\')\")", "<!(llvm-config --includedir)"],
      "cflags": [
        "-std=c++11"
      ],
      "libraries": [
        "<!@(llvm-config --ldflags)",
        "<!@(llvm-config --libs core jit native support bitwriter)"
      ]
    }
  ]
}
