/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

.intel_syntax noprefix

.text
/* exports _start, syscall? to other compilation units (files) */
.globl syscall0, syscall1, syscall2,
.globl syscall3, syscall4, syscall5,
.globl syscall6,

/* Registers %rbp, %rbx and %r12 through %r15 "belong" to the calling
 * function and the called function is required to preserve their values. */

syscall0:
    mov     rax, rdi	/* rax (syscall number) = func param 1 (rdi) */
    syscall
    ret

syscall1:
    mov     rax, rdi	/* rax (syscall number) = func param 1 (rdi) */
    mov     rdi, rsi	/* rdi (syscall param 1) = func param 2 (rsi) */
    syscall
    ret

syscall2:
    mov     rax, rdi	/* rax (syscall number) = func param 1 (rdi) */
    mov     rdi, rsi	/* rdi (syscall param 1) = func param 2 (rsi) */
    mov     rsi, rdx	/* rsi (syscall param 2) = func param 3 (rdx) */
    syscall
    ret

syscall3:
    mov     rax, rdi	/* rax (syscall number) = func param 1 (rdi) */
    mov     rdi, rsi	/* rdi (syscall param 1) = func param 2 (rsi) */
    mov     rsi, rdx	/* rsi (syscall param 2) = func param 3 (rdx) */
    mov     rdx, rcx	/* rdx (syscall param 3) = func param 4 (rcx) */
    syscall
    ret

syscall4:
    mov     rax, rdi	/* rax (syscall number) = func param 1 (rdi) */
    mov     rdi, rsi	/* rdi (syscall param 1) = func param 2 (rsi) */
    mov     rsi, rdx	/* rsi (syscall param 2) = func param 3 (rdx) */
    mov     rdx, rcx	/* rdx (syscall param 3) = func param 4 (rcx) */
    mov     r10, r8	    /* r10 (syscall param 4) = func param 5 (r8) */
    syscall
    ret

syscall5:
    mov     rax, rdi	/* rax (syscall number) = func param 1 (rdi) */
    mov     rdi, rsi	/* rdi (syscall param 1) = func param 2 (rsi) */
    mov     rsi, rdx	/* rsi (syscall param 2) = func param 3 (rdx) */
    mov     rdx, rcx	/* rdx (syscall param 3) = func param 4 (rcx) */
    mov     r10, r8	    /* r10 (syscall param 4) = func param 5 (r8) */
    mov     r8, r9	    /* r8 (syscall param 5) = func param 6 (r9) */
    syscall
    ret

syscall6:
    mov     rax, rdi	/* rax (syscall number) = func param 1 (rdi) */
    mov     rdi, rsi	/* rdi (syscall param 1) = func param 2 (rsi) */
    mov     rsi, rdx	/* rsi (syscall param 2) = func param 3 (rdx) */
    mov     rdx, rcx	/* rdx (syscall param 3) = func param 4 (rcx) */
    mov     r10, r8	    /* r10 (syscall param 4) = func param 5 (r8) */
    mov     r8, r9	    /* r8 (syscall param 5) = func param 6 (r9) */
    mov     r9, [rsp+8]	/* r9 (syscall param 6) = func param 7 (sp+8) */
    syscall
    ret
