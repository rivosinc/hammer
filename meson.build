# SPDX-FileCopyrightText: 2023 Rivos Inc.
#
# SPDX-License-Identifier: Apache-2.0

project('hammer lib', 'cpp',
        default_options : [
          'cpp_std=c++20',
          'warning_level=2', # https://github.com/mesonbuild/meson/issues/7748
          'werror=true',
          'default_library=static'
          ]
        )

spike_install_dir = get_option('spike_install_dir')

if spike_install_dir == ''
  if build_machine.system() == 'linux'
    spike_release_version = 'latest'

    lsb_release = find_program('lsb_release')
    lsb_release_ret = run_command(lsb_release, ['-ds'], check: true)

    if lsb_release_ret.stdout().contains('Ubuntu')
      distro = 'ubuntu.20.04'
    elif lsb_release_ret.stdout().contains('CentOS')
      distro = 'centos.7'
    endif

    spike_install_dir = '/tools/foss/riscv-isa-sim/dv-release/latest' / distro

  elif build_machine.system() == 'darwin'
    spike_release_version = ''

    distro = 'darwin'

    spike_install_dir = '/Users/joy/workspace/dv-release/' / distro
  endif
endif

spike_lib_directory = spike_install_dir /  'lib/'

spike_include_directory = spike_install_dir / 'include/'

thread_libdep = dependency('threads')

cpp = meson.get_compiler('cpp')
riscv_libdep = cpp.find_library('riscv', dirs: spike_lib_directory, required: true)

spike_incdirs = include_directories([
                                      spike_include_directory + 'softfloat',
                                      spike_include_directory + 'fesvr',
                                      spike_include_directory,
                                      '.'
                                    ])

hammer_lib = library('hammer',
                      'hammer.cpp',
                      include_directories : spike_incdirs,
                      dependencies : [riscv_libdep],
                      install : true
                      )

python3_instance = import('python').find_installation('python3')
pybind11_config = find_program('pybind11-config')
pybind11_config_ret = run_command(pybind11_config, ['--includes'], check: true)
pybind11 = declare_dependency(
    include_directories: [pybind11_config_ret.stdout().split('-I')[-1].strip()],
)

python3 = dependency('python3', )

hammer_pylib = python3_instance.extension_module('hammer',
    sources: ['hammer_pybind.cpp', 'hammer.cpp',],
    include_directories : spike_incdirs,
    dependencies : [python3, pybind11, riscv_libdep],
    link_language : 'cpp',
    override_options: [
        'cpp_rtti=true',
    ],
    cpp_args: [
      '-Wno-macro-redefined', # silence complaint about HAVE_DLOPEN set in both Spike and pybind11
      '-Wno-nonportable-include-path' # silence complaint about spike/VERSION and include <version> in pybind11 clashing
    ],
    install : true
)

install_headers('hammer.h', 'hammer_enums.h')

subdir('tests')
subdir('pytests')
