
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

hammer_tests = [
                ['get GPR, FPR, CSR and VecReg values', 'pytest000'],
                ['set GPR values', 'pytest001'],
                ['get/set memory contents', 'pytest002'],
                ['set PC', 'pytest003']
               ]

foreach hammer_test : hammer_tests
  test_description = hammer_test[0]
  test_name = hammer_test[1]

  pytest = find_program(test_name + '.py').full_path()

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

  test(
    'Python ' + test_description,
    import('python').find_installation('python3'),
    args : [pytest, test_input],
    env : dyld_lib_path
    )

endforeach
