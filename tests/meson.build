riscv_compiler = find_program('riscv64-unknown-elf-gcc', required : true)

linker_script_parameter = '-T' + meson.current_source_dir() + '/testinput.link.ld'

riscv_compiler_parameters = [
                            '-static',
                            '-mcmodel=medany',
                            '-fvisibility=hidden',
                            '-nostdlib',
                            '-nostartfiles',
                            '-Wa,-march=rv64gcv',
                            '-std=gnu99',
                            '-O0',
                            linker_script_parameter,
]

if build_machine.system() == 'darwin'
  dyld_lib_path = ['DYLD_LIBRARY_PATH=' + meson.source_root() + '/spike/build']
else
  dyld_lib_path = []
endif

hammer_tests = [
                ['get GPR, CSR and VecReg values', 'test000'],
                ['set GPR values', 'test001'],
                ['get/set memory contents', 'test002'],
                ['set PC', 'test003'],
               ]

foreach hammer_test : hammer_tests
  test_description = hammer_test[0]
  test_name = hammer_test[1]

  infile = test_name + '.input.S'
  outfile = test_name + '.input.elf'

  test_input = custom_target(test_name,
                          output : outfile,
                          input : infile,
                          command : [
                            riscv_compiler,
                            riscv_compiler_parameters,
                            '@INPUT@',
                            '-o',
                            '@OUTPUT@'
                            ]
                          )

  exe = executable(test_name,
                   test_name + '.cpp',
                   link_with : hammer_lib,
                   include_directories : spike_incdirs,
                   dependencies : [riscv_libdep]
  )
  test('C++ ' + test_description,
        exe, args : [test_input], env : dyld_lib_path)

endforeach
