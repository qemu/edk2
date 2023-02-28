#------------------------------------------------------------------------------
#
# Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
#------------------------------------------------------------------------------

#include <AsmMacroIoLib.h>

#/**
#   Routine for switching stacks with 2 parameters
#
# @param      EntryPoint     Entry point to be invoked after switching stack
# @param      Context1       Parameter1 for entry point.
# @param      Context2       Parameter2 for entry point.
# @param      NewStack       The pointer to the new stack.
#
# @return     None
#
#**/
#VOID
#EFIAPI
#PeiSwitchStacks (
#  IN      SWITCH_STACK_ENTRY_POINT  EntryPoint,
#  IN      VOID                      *Context1   OPTIONAL,
#  IN      VOID                      *Context2   OPTIONAL,
#  IN      VOID                      *NewStack
#  )
#
ASM_FUNC(PeiSwitchStacks)
    mov   sp, r3
    push {r1}
    push {r2}
    push {r0}
    pop  {r3}
    pop  {r1}
    pop  {r0}
    bx   r3
