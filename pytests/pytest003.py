#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2022 - 2026 Rivos Inc.
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

    hammer = pyhammer.Hammer("RV64GCV", "MSU", "vlen:512,elen:32", hart_ids, [mem_layout], elf_name,
                             None)

    for _ in range(8):
        hammer.single_step(0)

    current_PC = hammer.get_PC(0)
    current_x1 = hammer.get_gpr(0, 1)
    current_x2 = hammer.get_gpr(0, 2)
    current_x3 = hammer.get_gpr(0, 3)

    if (current_PC != 0x80000008):
        sys.exit(f'Unexpected PC: 0x{current_PC:x}')

    if (current_x1 != 1):
        sys.exit(f'Unexpected x1: {current_x1}')

    if (current_x2 != 2):
        sys.exit(f'Unexpected x2: {current_x2}')

    if (current_x3 != 3):
        sys.exit(f'Unexpected x3: {current_x3}')

    # Skip over the bad instructions
    next_PC = current_PC + (4 * 8)
    hammer.set_PC(0, next_PC)
    hammer.single_step(0)

    current_PC = hammer.get_PC(0)
    current_x3 = hammer.get_gpr(0, 3)

    if (current_PC != (next_PC + 2)):
        sys.exit(f'Unexpected PC after skipping over the bad instruction: {current_PC:x}')

    if (current_x3 != 4):
        sys.exit(f'Unexpected x3 after skipping over the bad instruction: {current_x3}')

    pass


if __name__ == '__main__':
    main()
