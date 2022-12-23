// SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "hammer.h"

#include "fesvr/option_parser.h"
#include "riscv/cachesim.h"
#include "riscv/decode.h"

#include <vector>

// FIXME: This function exists in Spike as a static function. We shouldn't have to
// copy it out here but sim_t requires it as an argument.
static std::vector<std::pair<reg_t, mem_t *>> make_mems(const std::vector<mem_cfg_t> &layout) {
  std::vector<std::pair<reg_t, mem_t *>> mems;
  mems.reserve(layout.size());
  for (const auto &cfg : layout) {
    mems.push_back(std::make_pair(cfg.base, new mem_t(cfg.size)));
  }
  return mems;
}

Hammer::Hammer(const char *isa, const char *privilege_levels, const char *vector_arch,
               std::vector<int> hart_ids, std::vector<mem_cfg_t> memory_layout,
               const std::string target_binary, const std::optional<uint64_t> start_pc) {
  // Expose these only if needed
  std::vector<std::pair<reg_t, abstract_device_t *>> plugin_devices;
  debug_module_config_t dm_config = {.progbufsize = 2,
                                     .max_sba_data_width = 0,
                                     .require_authentication = false,
                                     .abstract_rti = 0,
                                     .support_hasel = true,
                                     .support_abstract_csr_access = true,
                                     .support_haltgroups = true,
                                     .support_impebreak = true};
  const char *log_path = nullptr;
  const char *dtb_file = nullptr;
  FILE *cmd_file = nullptr;

  std::pair<reg_t, reg_t> initrd_bounds{0, 0};
  const char *bootargs = nullptr;
  bool real_time_clint = false;

  reg_t num_pmpregions = 16;

  endianness_t endinaness = endianness_little;

  cfg_t cfg = cfg_t(initrd_bounds, bootargs, isa, privilege_levels, vector_arch, endinaness, num_pmpregions, memory_layout,
                    hart_ids, real_time_clint);

  if (start_pc.has_value()) {
    cfg.start_pc = start_pc.value();
  }

  std::vector<std::pair<reg_t, mem_t *>> mems = make_mems(memory_layout);

  std::vector<std::string> htif_args;
  htif_args.push_back(target_binary);

  bool halted = false;
  bool dtb_enabled = true;
  bool socket_enabled = false;

  simulator = new sim_t(&cfg, halted, mems, plugin_devices, htif_args, dm_config, log_path,
                        dtb_enabled, dtb_file, socket_enabled, cmd_file);

  // Initializes everything
  simulator->start();
}

Hammer::~Hammer() { delete simulator; }

reg_t Hammer::get_gpr(uint8_t hart_id, uint8_t gpr_id) {
  assert(gpr_id < NXPR);

  processor_t *hart = simulator->get_core(hart_id);
  state_t *hart_state = hart->get_state();

  return hart_state->XPR[gpr_id];
}

void Hammer::set_gpr(uint8_t hart_id, uint8_t gpr_id, reg_t new_gpr_value) {
  assert(gpr_id < NXPR);

  processor_t *hart = simulator->get_core(hart_id);
  state_t *hart_state = hart->get_state();

  hart_state->XPR.write(gpr_id, new_gpr_value);
}

uint64_t Hammer::get_fpr(uint8_t hart_id, uint8_t fpr_id) {
  assert(fpr_id < NFPR);

  processor_t *hart = simulator->get_core(hart_id);
  state_t *hart_state = hart->get_state();
  freg_t fpr_value = hart_state->FPR[fpr_id];

  return fpr_value.v[0];
}

reg_t Hammer::get_PC(uint8_t hart_id) {
  processor_t *hart = simulator->get_core(hart_id);
  state_t *hart_state = hart->get_state();

  return hart_state->pc;
}

void Hammer::set_PC(uint8_t hart_id, reg_t new_pc_value) {
  processor_t *hart = simulator->get_core(hart_id);
  state_t *hart_state = hart->get_state();

  hart_state->pc = new_pc_value;
}

reg_t Hammer::get_csr(uint8_t hart_id, uint32_t csr_id) {
  processor_t *hart = simulator->get_core(hart_id);
  return hart->get_csr(csr_id);
}

void Hammer::single_step(uint8_t hart_id) {
  processor_t *hart = simulator->get_core(hart_id);
  hart->step(1);
}

uint32_t Hammer::get_flen(uint8_t hart_id) {
  processor_t *hart = simulator->get_core(hart_id);
  return hart->get_flen();
}

reg_t Hammer::get_vlen(uint8_t hart_id) {
  processor_t *hart = simulator->get_core(hart_id);
  return hart->VU.get_vlen();
}

reg_t Hammer::get_elen(uint8_t hart_id) {
  processor_t *hart = simulator->get_core(hart_id);
  return hart->VU.get_elen();
}

std::vector<uint64_t> Hammer::get_vector_reg(uint8_t hart_id, uint8_t vector_reg_id) {
  assert(vector_reg_id < NVPR);

  processor_t *hart = simulator->get_core(hart_id);
  uint32_t vlen = hart->VU.get_vlen();

  std::vector<uint64_t> vector_reg_value;

  for (uint32_t i = 0; i < (vlen / 64); ++i) {
    vector_reg_value.push_back(hart->VU.elt<uint64_t>(vector_reg_id, i));
  }

  return vector_reg_value;
}

