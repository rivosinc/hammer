// SPDX-FileCopyrightText: Copyright (c) 2022 by Rivos Inc.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
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
