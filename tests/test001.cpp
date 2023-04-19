// SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "hammer.h"

int main(int argc, char *argv[]) {
  assert(argc == 2);
  const std::string target_binary = argv[1];

  std::vector<mem_cfg_t> memory_layout;
  // This is what Spike sets it to
  memory_layout.push_back(mem_cfg_t(reg_t(DRAM_BASE), reg_t(2048) << 20));

  std::vector<size_t> hart_ids{0};

  Hammer hammer = Hammer("RV64GCV", "MSU", "vlen:512,elen:64", hart_ids, memory_layout,
                         target_binary, std::nullopt);

  for (uint32_t i = 0; i < 7; ++i) {
    hammer.single_step(0);
  }

  uint64_t current_pc = hammer.get_PC(0);
  uint64_t current_x15 = hammer.get_gpr(0, 15);
  uint64_t current_x16 = hammer.get_gpr(0, 16);

  if (current_pc != 0x80000004) {
    printf("Unexpected PC: 0x%" PRIx64 "\n", current_pc);
    exit(1);
  }

  if (current_x15 != 15) {
    printf("Unexpected x15: 0x%" PRIx64 "\n", current_x15);
    exit(1);
  }

  if (current_x16 != 16) {
    printf("Unexpected x16: 0x%" PRIx64 "\n", current_x16);
    exit(1);
  }

  // Test set_gpr()
  hammer.set_gpr(0, 15, 2015);
  hammer.set_gpr(0, 16, 2016);

  current_x15 = hammer.get_gpr(0, 15);
  current_x16 = hammer.get_gpr(0, 16);

  if (current_x15 != 2015) {
    printf("Unexpected x15: 0x%" PRIx64 "\n", current_x15);
    exit(1);
  }

  if (current_x16 != 2016) {
    printf("Unexpected x16: 0x%" PRIx64 "\n", current_x16);
    exit(1);
  }

  for (uint32_t i = 0; i < 2; ++i) {
    hammer.single_step(0);
  }

  uint64_t current_x5 = hammer.get_gpr(0, 5);
  uint64_t current_x6 = hammer.get_gpr(0, 6);

  if (current_x5 != 2015) {
    printf("Unexpected x5: 0x%" PRIx64 "\n", current_x5);
    exit(1);
  }

  if (current_x6 != 2016) {
    printf("Unexpected x6: 0x%" PRIx64 "\n", current_x6);
    exit(1);
  }
}
