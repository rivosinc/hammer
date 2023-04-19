// SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "hammer_enums.h"
#include "riscv/mmu.h"
#include "riscv/sim.h"

#include <iostream>

class Hammer {
 public:
  Hammer(const char *isa, const char *privilege_levels, const char *vector_arch,
         std::vector<size_t> hart_ids, std::vector<mem_cfg_t> memory_layout,
         const std::string target_binary, const std::optional<uint64_t> start_pc = std::nullopt);
  ~Hammer();

  reg_t get_gpr(uint8_t hart_id, uint8_t gpr_id);
  void set_gpr(uint8_t hart_id, uint8_t gpr_id, reg_t new_gpr_value);

  uint64_t get_fpr(uint8_t hart_id, uint8_t fpr_id);

  reg_t get_PC(uint8_t hart_id);
  void set_PC(uint8_t hart_id, reg_t new_pc_value);

  reg_t get_csr(uint8_t hart_id, uint32_t csr_id);

  void single_step(uint8_t hart_id);

  uint32_t get_flen(uint8_t hart_id);

  reg_t get_vlen(uint8_t hart_id);
  reg_t get_elen(uint8_t hart_id);

  std::vector<uint64_t> get_vector_reg(uint8_t hart_id, uint8_t vector_reg_id);

  template <typename T>
  std::optional<std::vector<T>> get_memory_at_VA(uint8_t hart_id, uint64_t virtual_address,
                                                 size_t num_bytes_to_read) {
    if (num_bytes_to_read % sizeof(T))
      return std::nullopt;
    mmu_t *mmu = simulator->get_core(hart_id)->get_mmu();
    std::vector<T> data;
    try {
      for (size_t i = 0; i < num_bytes_to_read; i += sizeof(T))
        data.push_back(mmu->load<T>(virtual_address + i));
    } catch (trap_t &t) {
      return std::nullopt;
    }
    return data;
  }

  template <typename T>
  int set_memory_at_VA(uint8_t hart_id, uint64_t virtual_address,
                       const std::vector<T> &memory_contents) {
    mmu_t *mmu = simulator->get_core(hart_id)->get_mmu();
    try {
      for (auto &i : memory_contents) {
        mmu->store<T>(virtual_address, i);
        virtual_address += sizeof(T);
      }
    } catch (trap_t &t) {
      return -EFAULT;
    }
    return 0;
  }

  void hello_world() { printf("Hammer: Hello World.\n"); }

 private:
  sim_t *simulator;
};
