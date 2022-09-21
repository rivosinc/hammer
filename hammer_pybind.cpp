// SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "hammer.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PYBIND11_MODULE(hammer, m) {
  m.doc() = "Hammer Python Interface";

  pybind11::class_<mem_cfg_t>(m, "mem_cfg_t").def(pybind11::init<reg_t, reg_t>());

  pybind11::class_<mem_t>(m, "mem_t").def(pybind11::init<reg_t>());

  pybind11::class_<Hammer>(m, "Hammer")
      .def(pybind11::init<const char *, const char *, const char *, std::vector<int>,
                          std::vector<mem_cfg_t>, const std::string,
                          const std::optional<uint64_t>>())
      .def("hello_world", &Hammer::hello_world)
      .def("get_gpr", &Hammer::get_gpr)
      .def("set_gpr", &Hammer::set_gpr)
      .def("get_PC", &Hammer::get_PC)
      .def("set_PC", &Hammer::set_PC)
      .def("get_csr", &Hammer::get_csr)
      .def("get_vlen", &Hammer::get_vlen)
      .def("get_elen", &Hammer::get_elen)
      .def("get_vector_reg", &Hammer::get_vector_reg)
      .def("get_memory_at_VA", &Hammer::get_memory_at_VA)
      .def("set_memory_at_VA", &Hammer::set_memory_at_VA)
      .def("single_step", &Hammer::single_step);

  // Reference:
  // https://stackoverflow.com/questions/47893832/pybind11-global-level-enum
  pybind11::enum_<PlatformDefines>(m, "PlatformDefines")
      .value("DramBase", DramBase)
      .export_values();
}
