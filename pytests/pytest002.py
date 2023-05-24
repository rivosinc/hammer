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

    # Tests Hammer::set/get_memory_at_VA()
    # Reads memory using Hammer::get_memory_at_VA(), single steps a load that
    # reads the same address and compares values.
    virtual_address = 0x80001000
    memory_contents = hammer.get_memory_at_VA(0, virtual_address, 8, 1)

    if (memory_contents is None):
        sys.exit("ERROR: Unable to read memory")

    value_at_address = 0
    i = 0
    for byte_value in memory_contents:
        value_at_address |= (byte_value << (i * 8))
        i = i + 1

    for _ in range(8):
        hammer.single_step(0)

    current_x2 = hammer.get_gpr(0, 2)

    if (value_at_address != current_x2):
        print(f"get_memory_at_VA() returned a different value {value_at_address:x} from"
              "the load instruction which returned {current_x2}")
        sys.exit(1)

    memory_contents = hammer.get_memory_at_VA(0, virtual_address, 8, 8)
    if (memory_contents[0] != current_x2):
        sys.exit(f'unexpected data read: 0x{memory_contents[0]:x}')

    memory_contents = hammer.get_memory_at_VA(0, virtual_address, 8, 4)
    memory_contents[0] |= memory_contents[1] << 32
    if (memory_contents[0] != current_x2):
        sys.exit(f'unexpected data read: 0x{memory_contents[0]:x}')

    memory_contents = hammer.get_memory_at_VA(0, virtual_address, 8, 2)
    memory_contents[0] |= memory_contents[1] << 16
    memory_contents[0] |= memory_contents[2] << 32
    memory_contents[0] |= memory_contents[3] << 48
    if (memory_contents[0] != current_x2):
        sys.exit(f'unexpected data read: 0x{memory_contents[0]:x}')

    # Writes memory using Hammer::set_memory_at_VA(), single steps a load that
    # reads the same address and compares values.
    new_memory_contents = [0xab, 0xcd, 0xef, 0xde, 0x11, 0x22, 0x33, 0x44]
    new_value_at_address = 0
    i = 0
    for byte_value in new_memory_contents:
        new_value_at_address |= (byte_value << (i * 8))
        i = i + 1

    if (hammer.set_memory_at_VA(0, virtual_address, new_memory_contents, 1)):
        sys.exit("ERROR: set_memory_at_VA() failed")

    # Step the next load that will read the same location
    hammer.single_step(0)

    current_x2 = hammer.get_gpr(0, 2)

    if (new_value_at_address != current_x2):
        print(f"set_memory_at_VA() updated memory with {new_value_at_address:x} "
              "the load instruction returned {current_x2:x}")
        sys.exit(1)

    # Tests the fail case.
    memory_contents = hammer.get_memory_at_VA(0, 0xabcdabcdabcd, 1, 1)
    if (memory_contents is not None):
        sys.exit("ERROR: Expected memory load to fail.")

    if (hammer.set_memory_at_VA(0, 0xabcdabcdabcd, new_memory_contents, 1) == 0):
        sys.exit("ERROR: Expected memory store to fail.")

    # Test set of various sizes (8/4/2)
    new_memory_contents = [0xdeadbeef01234567]
    if (hammer.set_memory_at_VA(0, virtual_address, new_memory_contents, 8)):
        sys.exit("ERROR: set_memory_at_VA() failed")
    memory_contents = hammer.get_memory_at_VA(0, virtual_address, 8, 8)
    if (memory_contents[0] != new_memory_contents[0]):
        sys.exit(f'unexpected data read: 0x{memory_contents[0]:x}')

    new_memory_contents = [0xdeadbeef]
    if (hammer.set_memory_at_VA(0, virtual_address, new_memory_contents, 4)):
        sys.exit("ERROR: set_memory_at_VA() failed")
    memory_contents = hammer.get_memory_at_VA(0, virtual_address, 4, 4)
    if (memory_contents[0] != new_memory_contents[0]):
        sys.exit(f'unexpected data read: 0x{memory_contents[0]:x}')

    new_memory_contents = [0xdead]
    if (hammer.set_memory_at_VA(0, virtual_address, new_memory_contents, 2)):
        sys.exit("ERROR: set_memory_at_VA() failed")
    memory_contents = hammer.get_memory_at_VA(0, virtual_address, 2, 2)
    if (memory_contents[0] != new_memory_contents[0]):
        sys.exit(f'unexpected data read: 0x{memory_contents[0]:x}')

    # Invalid stride
    try:
        hammer.set_memory_at_VA(0, virtual_address, new_memory_contents, 3)
    except ValueError:
        pass
    try:
        hammer.get_memory_at_VA(0, virtual_address, 2, 5)
    except ValueError:
        pass

    pass


if __name__ == '__main__':
    main()
