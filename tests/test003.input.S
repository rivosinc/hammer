// SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

  .text

  .global _start
_start:

  li  x1, 1
  li  x2, 2
  add x3, x2, x1; # x3 = x2 + x1 = 3

  .rept 8
    .4byte 0xbad1bad2 # bad instruction we're going to skip over
  .endr

  c.add x3, x1 # x3 = x3 + x1 = 4

  # Magic instruction to terminate the run.
  lui x0, 0xdeadb

  .data

tdat:
  .dword 0x1817161514131110
  .dword 0x2827262524232120
  .dword 0x3837363534333130
  .dword 0x4847464544434140
