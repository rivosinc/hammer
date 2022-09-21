#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).parent.parent.resolve()))

# hammer needs to be built for builddir/ to exist.
import builddir.hammer as pyhammer


def main():
    if len(sys.argv) != 2:
        sys.exit("Unexpected number of parameters passed to test.")

    elf_name = sys.argv[1]

    mem_size = 2048 << 20
    mem_layout = pyhammer.mem_cfg_t(pyhammer.DramBase, mem_size)
    hart_ids = [0]

    mems = [(pyhammer.DramBase, pyhammer.mem_t(mem_size))]

    hammer = pyhammer.Hammer("RV64GCV", "MSU", "vlen:512,elen:64", hart_ids, [mem_layout], elf_name,
                             None)

    vlen = hammer.get_vlen(0)
    if (vlen != 512):
        sys.exit(f"Unexpected vlen: {vlen}")

    elen = hammer.get_elen(0)
    if (elen != 64):
        sys.exit(f"Unexpected elen: {elen}")

    #  hammer.hello_world()

    for _ in range(8):
        hammer.single_step(0)

    current_pc = hammer.get_PC(0)
    current_x1 = hammer.get_gpr(0, 1)
    current_x2 = hammer.get_gpr(0, 2)
    current_x3 = hammer.get_gpr(0, 3)

    if (current_pc != 0x80000008):
        sys.exit(f'Unexpected PC: 0x{current_pc:x}')

    if (current_x1 != 1):
        sys.exit(f'Unexpected x1: {current_x1}')

    if (current_x2 != 2):
        sys.exit(f'Unexpected x2: {current_x2}')

    if (current_x3 != 3):
        sys.exit(f'Unexpected x3: {current_x3}')

    for _ in range(12):
        hammer.single_step(0)

    # TODO: add a read of the mstatus CSR which is set by this point
    current_x1 = hammer.get_gpr(0, 1)
    if (current_x1 != 4):
        sys.exit(f'Unexpected x1 after vsetivli {current_x1}')

    v1 = hammer.get_vector_reg(0, 1)
    if len(v1) != 8:
        sys.exit(f"Unexpected number of 64 bit elements in v1: {len(v1)}")

    if (v1[0] != 0x1000 or v1[1] != 0x0 or v1[2] != 0x0 or v1[3] != 0):
        sys.exit(f"Unexpected value in v1: 0x{v1[3]:x}{v1[2]:x}{v1[1]:x}{v1[0]:x}")

    v2 = hammer.get_vector_reg(0, 2)
    if len(v2) != 8:
        sys.exit(f"Unexpected number of 64 bit elements in v2: {len(v2)}")

    if (v2[0] != 0x2000 or v2[1] != 0x0 or v2[2] != 0x0 or v2[3] != 0):
        sys.exit(f"Unexpected value in v2: 0x{v2[3]:x}{v2[2]:x}{v2[1]:x}{v2[0]:x}")

    v3 = hammer.get_vector_reg(0, 3)
    if len(v3) != 8:
        sys.exit(f"Unexpected number of 64 bit elements in v3: {len(v3)}")

    if (v3[0] != 0x3000 or v3[1] != 0x0 or v3[2] != 0x0 or v3[3] != 0):
        sys.exit(f"Unexpected value in v3: 0x{v3[3]:x}{v3[2]:x}{v3[1]:x}{v3[0]:x}")

    pass


if __name__ == '__main__':
    main()
