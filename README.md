<!--
SPDX-FileCopyrightText: 2023 Rivos Inc.

SPDX-License-Identifier: Apache-2.0
-->

# Hammer

[![REUSE status](https://api.reuse.software/badge/github.com/rivosinc/hammer)](https://api.reuse.software/info/github.com/rivosinc/hammer)

Infrastructure to drive Spike (RISC-V ISA Simulator) in cosim mode. Hammer provides a C++ and Python interface to interact with Spike.

## Dependencies

* `python3`
* `meson` is the build infrastructure.
* `pybind11` Python module:
```
python3 -m pip install pybind11
```
* Libraries and headers from Spike

In the Spike repo run:

```
mkdir build
cd build/
../configure --with-isa=RV64GCV --without-boost --without-boost-asio --without-boost-regex --prefix <SPIKE INSTALL LOCATION>
make
make install
```

The SHA of the last tested Spike version is included in the name of the patch file.

## Building Hammer and Running Tests

```
meson setup builddir --native-file native-file.txt --buildtype release -Dspike_install_dir=<SPIKE INSTALL LOCATION>
meson compile -C builddir
meson test -C builddir
```

*NOTE: Finish the compile step before running the pytests.*

The pytests require the compile step be complete before being run because
they depend on the `hammer.pylib` being built. I couldn't figure out a way to add
a dependency between the pylib and the pytests in meson.

### Release procedure

```
meson setup builddir --native-file native-file.txt -Dspike_install_dir=<SPIKE INSTALL LOCATION> --buildtype release --prefix <INSTALL PATH>
meson compile -C builddir
meson install -C builddir
```
