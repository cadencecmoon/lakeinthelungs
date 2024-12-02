/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

/* TODO */

.intel_syntax noprefix
.text
.globl make_fcontext
.type make_fcontext,@function
.align 16

make_fcontext:
    /* first arg of make_fcontext() == top of context-stack */
    mov     rax, rdi

    /* shift address in RAX to lower 16 byte boundary */
    and     rax, -16

    /* reserve space for context-data on context-stack 
     * size for fc_mxcsr .. RIP + return-address for context-function
     * on context-function entry: (RSP -0x8) % 16 == 0 */
    lea     rax, [rax-48h]

    /* third arg of make_fcontext() == address of context-function */
    mov     [rax+38h], rdx

    /* save MMX control-word and status-word */
    stmxcsr rax
    /* save x87 control-word */
    fnstcw  [rax+4h]

    /* compute abs address of label finish */
    lea     rcx, finish

    /* save address of finish as return-address for context-function
     * will be entered after context-function returns */
    movq    %rcx,   0x40(%rax)

    /* return pointer to context-data */
    ret

finish:
    /* exit code is zero */
    xor     rdi, rdi

    /* exit application */
    mov     rax, 231 /* SYS_exit_group */
    syscall
    hlt
.size make_fcontext,.-make_fcontext

/* mark that we don't need executable stack */
.section .note.GNU-stack,"",%progbits
