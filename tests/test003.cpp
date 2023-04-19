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

  std::vector<std::string> htif_args;
  htif_args.push_back(target_binary);

  std::vector<size_t> hart_ids{0};

  Hammer hammer = Hammer("RV64GCV", "MSU", "vlen:512,elen:64", hart_ids, memory_layout,
                         target_binary, std::nullopt);

  // hammer.hello_world();
  for (uint32_t i = 0; i < 8; ++i) {
    hammer.single_step(0);
  }

  uint64_t current_PC = hammer.get_PC(0);
  uint64_t current_x1 = hammer.get_gpr(0, 1);
  uint64_t current_x2 = hammer.get_gpr(0, 2);
  uint64_t current_x3 = hammer.get_gpr(0, 3);

  if (current_PC != 0x80000008) {
    printf("Unexpected PC: 0x%" PRIx64 "\n", current_PC);
    exit(1);
  }

  if (current_x1 != 1) {
    printf("Unexpected x1: 0x%" PRIx64 "\n", current_x1);
    exit(1);
  }

  if (current_x2 != 2) {
    printf("Unexpected x2: 0x%" PRIx64 "\n", current_x2);
    exit(1);
  }

  if (current_x3 != 3) {
    printf("Unexpected x3: 0x%" PRIx64 "\n", current_x3);
    exit(1);
  }

  // Skip over the bad data
  uint64_t next_PC = current_PC + (4 * 8);
  hammer.set_PC(0, next_PC);
  hammer.single_step(0);

  current_PC = hammer.get_PC(0);
  current_x3 = hammer.get_gpr(0, 3);

  if (current_PC != (next_PC + 2)) {
    printf("Unexpected PC after skipping over the bad instruction: 0x%" PRIx64 "\n", current_PC);
    exit(1);
  }

  if (current_x3 != 4) {
    printf("Unexpected x3 after skipping over the bad instruction: 0x%" PRIx64 "\n", current_x3);
    exit(1);
  }
}
