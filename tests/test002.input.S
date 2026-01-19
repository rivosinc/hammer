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

  la x1, tdat
  ld  x2, (x1)

  ld  x2, (x1)

  # Magic instruction to terminate the run.
  lui x0, 0xdeadb

  .data

tdat:
  .dword 0xcafecafecafecafe
  .dword 0xba11ba11ba11ba11
