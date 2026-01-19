/*
 * SPDX-FileCopyrightText: 2026 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// SPDX-FileCopyrightText: 2022 Rivos Inc.
//
// SPDX-License-Identifier: Apache-2.0

  .text

  .global _start
_start:

  li  x15, 15
  li  x16, 16

  mv  x5, x15
  mv  x6, x16

  # Magic instruction to terminate the run.
  lui x0, 0xdeadb

  .data

tdat:
  .dword 0x1817161514131110
  .dword 0x2827262524232120
  .dword 0x3837363534333130
  .dword 0x4847464544434140
