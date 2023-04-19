// SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "hammer.h"

constexpr uint64_t data_area_start_address = 0x80001000;

int main(int argc, char *argv[]) {
  assert(argc == 2);
  const std::string target_binary = argv[1];

  std::vector<mem_cfg_t> memory_layout;
  // This is what Spike sets it to
  memory_layout.push_back(mem_cfg_t(reg_t(DRAM_BASE), reg_t(2048) << 20));

  std::vector<size_t> hart_ids{0};

  Hammer hammer = Hammer("RV64GCV", "MSU", "vlen:512,elen:64", hart_ids, memory_layout,
                         target_binary, std::nullopt);

  // Tests Hammer::set/get_memory_at_VA()
  // single steps a load that reads a location, reads the same location with
  // get_memory_at_VA() and compares values.
  for (uint32_t i = 0; i < 8; ++i) {
    hammer.single_step(0);
  }
  uint64_t current_x2 = hammer.get_gpr(0, 2);

  uint64_t virtual_address = data_area_start_address;
  for (uint32_t bytes_to_read = 8; bytes_to_read > 0; --bytes_to_read) {
    auto memory_contents = hammer.get_memory_at_VA<uint8_t>(0, virtual_address, bytes_to_read);
    if (memory_contents.has_value() == false) {
      printf("ERROR: Unable to read memory\n");
      exit(1);
    }

    if (memory_contents.value().size() != bytes_to_read) {
      printf("ERROR: Bytes read did not match requested num bytes\n");
      exit(1);
    }

    uint64_t value_at_address = 0;
    uint8_t i = 0;
    for (uint64_t byte_value : memory_contents.value()) {
      value_at_address |= (byte_value << (i * 8));
      ++i;
    }

    uint64_t expected_value_at_address = current_x2 >> ((8 - bytes_to_read) * 8);
    if (expected_value_at_address != value_at_address) {
      printf("ERROR: %d bytes read from 0x%" PRIx64 " were 0x%" PRIx64
             " but "
             "expected were 0x%" PRIx64 "\n",
             bytes_to_read, virtual_address, value_at_address, expected_value_at_address);
      exit(1);
    }

    ++virtual_address;
  }

  // Writes memory using Hammer::set_memory_at_VA(), single steps a load that
  // reads the same address and compares values.
  std::vector<uint8_t> new_memory_contents{0xab, 0xcd, 0xef, 0xde, 0x11, 0x22, 0x33, 0x44};
  uint64_t new_value_at_address = 0;
  uint8_t i = 0;
  for (uint64_t byte_value : new_memory_contents) {
    new_value_at_address |= (byte_value << (i * 8));
    ++i;
  }

  virtual_address = data_area_start_address;
  if (hammer.set_memory_at_VA<uint8_t>(0, virtual_address, new_memory_contents)) {
    printf("ERROR: set_memory_at_VA() failed\n");
    exit(1);
  }

  // Step the next load that will read the same location
  hammer.single_step(0);

  current_x2 = hammer.get_gpr(0, 2);

  if (new_value_at_address != current_x2) {
    printf("set_memory_at_VA() updated memory with 0x%" PRIx64
           " but "
           "the load instruction returned 0x%" PRIx64 "\n",
           new_value_at_address, current_x2);
    exit(1);
  }

  // Tests the fail case.
  auto memory_contents = hammer.get_memory_at_VA<uint32_t>(0, 0xabcdabcdabcd, 1);
  if (memory_contents.has_value()) {
    printf("ERROR: Expected memory load to fail.\n");
    exit(1);
  }

  if (hammer.set_memory_at_VA<uint8_t>(0, 0xabcdabcdabcd, new_memory_contents) == 0) {
    printf("ERROR: Expected memory store to fail.\n");
    exit(1);
  }
}
