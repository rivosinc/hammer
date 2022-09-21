// SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

  .text

  .global _start
_start:

  li  x1, 1
  li  x2, 2
  add x3, x2, x1; # x3 = x2 + x1

  li  a3, 0xa00002200
  csrw    mstatus, a3

  vsetivli x1, 4, e32, m1

  lui a3, 1
  vmv.s.x v1, a3

  lui a3, 2
  vmv.s.x v2, a3

  lui a3, 0
  vmv.s.x v3, a3

  vadd.vv v3, v2, v1

  li x1, 0x3fc00000
  fmv.w.x f1, x1

  # Magic instruction to terminate the run.
  lui x0, 0xdeadb

  .data

tdat:
  .dword 0x1817161514131110
  .dword 0x2827262524232120
  .dword 0x3837363534333130
  .dword 0x4847464544434140
