// SPDX-FileCopyrightText: 2022 Rivos Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include "hammer.h"

int main(int argc, char *argv[]) {
  assert(argc == 2);
  const std::string target_binary = argv[1];

  std::vector<mem_cfg_t> memory_layout;
  // This is what Spike sets it to
  memory_layout.push_back(mem_cfg_t(reg_t(DRAM_BASE), reg_t(2048) << 20));

  std::vector<size_t> hart_ids{0};

  Hammer hammer = Hammer("RV64GCV", "MSU", "vlen:512,elen:32", hart_ids, memory_layout,
                         target_binary, std::nullopt);

  // hammer.hello_world();
  uint32_t flen = hammer.get_flen(0);
  if (flen != 64) {
    printf("Unexpected flen: %d\n", flen);
    exit(1);
  }

  uint32_t vlen = hammer.get_vlen(0);
  if (vlen != 512) {
    printf("Unexpected vlen: %d\n", vlen);
    exit(1);
  }

  uint32_t elen = hammer.get_elen(0);
  if (elen != 32) {
    printf("Unexpected elen: %d\n", elen);
    exit(1);
  }

  for (uint32_t i = 0; i < 8; ++i) {
    hammer.single_step(0);
  }

  uint64_t current_pc = hammer.get_PC(0);
  uint64_t current_x1 = hammer.get_gpr(0, 1);
  uint64_t current_x2 = hammer.get_gpr(0, 2);
  uint64_t current_x3 = hammer.get_gpr(0, 3);

  if (current_pc != 0x80000008) {
    printf("Unexpected PC: 0x%" PRIx64 "\n", current_pc);
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

  for (uint32_t i = 0; i < 13; ++i) {
    hammer.single_step(0);
  }

  reg_t current_mstatus = hammer.get_csr(0, MSTATUS_CSR);
  if (current_mstatus != 0x8000000a00002600) {
    printf("Unexpected mstatus: 0x%" PRIx64 "\n", current_mstatus);
    exit(1);
  }

  current_x1 = hammer.get_gpr(0, 1);
  if (current_x1 != 4) {
    printf("Unexpected x1: 0x%" PRIx64 " after vsetivli\n", current_x1);
    exit(1);
  }

  std::vector<uint64_t> v1 = hammer.get_vector_reg(0, 1);
  if (v1.size() != 8) {
    printf("Unexpected number of 64 bit elements in v1: %lu\n", v1.size());
    exit(1);
  }

  if (v1[0] != 0x1000 || v1[1] != 0x0 || v1[2] != 0x0 || v1[3] != 0) {
    printf("Unexpected value in v1: 0x%" PRIx64 "%" PRIx64 "%" PRIx64 "%" PRIx64 "\n", v1[3], v1[2],
           v1[1], v1[0]);
    exit(1);
  }

  std::vector<uint64_t> v2 = hammer.get_vector_reg(0, 2);
  if (v2.size() != 8) {
    printf("Unexpected number of 64 bit elements in v2: %lu\n", v2.size());
    exit(1);
  }

  if (v2[0] != 0x2000 || v2[1] != 0x0 || v2[2] != 0x0 || v2[3] != 0) {
    printf("Unexpected value in v2: 0x%" PRIx64 "%" PRIx64 "%" PRIx64 "%" PRIx64 "\n", v2[3], v2[2],
           v2[1], v2[0]);
    exit(1);
  }

  std::vector<uint64_t> v3 = hammer.get_vector_reg(0, 3);
  if (v3.size() != 8) {
    printf("Unexpected number of 64 bit elements in v3: %lu\n", v3.size());
    exit(1);
  }

  if (v3[0] != 0x3000 || v3[1] != 0x0 || v3[2] != 0x0 || v3[3] != 0) {
    printf("Unexpected value in v3: 0x%" PRIx64 "%" PRIx64 "%" PRIx64 "%" PRIx64 "\n", v3[3], v3[2],
           v3[1], v3[0]);
    exit(1);
  }

  for (uint32_t i = 0; i < 2; ++i) {
    hammer.single_step(0);
  }

  uint64_t current_f1 = hammer.get_fpr(0, 1);
  if (current_f1 != 0xffffffff3fc00000) {
    printf("Unexpected f1: 0x%" PRIx64 "\n", current_f1);
    exit(1);
  }
}
