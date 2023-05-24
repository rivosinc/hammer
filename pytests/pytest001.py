#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# SPDX-FileCopyrightText: 2022 Rivos Inc.
#
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

    for _ in range(7):
        hammer.single_step(0)

    current_pc = hammer.get_PC(0)
    current_x15 = hammer.get_gpr(0, 15)
    current_x16 = hammer.get_gpr(0, 16)

    if (current_pc != 0x80000004):
        sys.exit(f'Unexpected PC: 0x{current_pc:x}')

    if (current_x15 != 15):
        sys.exit(f'Unexpected x15: {current_x15}')

    if (current_x16 != 16):
        sys.exit(f'Unexpected x16: {current_x16}')

    hammer.set_gpr(0, 15, 2015)
    hammer.set_gpr(0, 16, 2016)

    current_x15 = hammer.get_gpr(0, 15)
    current_x16 = hammer.get_gpr(0, 16)

    if (current_x15 != 2015):
        sys.exit(f"Unexpected x15: {current_x15}")

    if (current_x16 != 2016):
        sys.exit(f"Unexpected x16: {current_x16}")

    for _ in range(2):
        hammer.single_step(0)

    current_x5 = hammer.get_gpr(0, 5)
    current_x6 = hammer.get_gpr(0, 6)

    if (current_x5 != 2015):
        sys.exit(f"Unexpected x5: {current_x5}")

    if (current_x6 != 2016):
        sys.exit(f"Unexpected x6: {current_x6}")

    pass


if __name__ == '__main__':
    main()
