/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

.intel_syntax noprefix

.text
.globl _start,

_start:
    xor     rbp, rbp
    pop     rdi /* rdi = argc */

    /* the pop instruction already added 8 to rsp */
    mov     rsi, rsp

    /* zero the last 4 bits of rsp, aligning it to 16 bytes
     * same as "and rsp, 0xfffffffffffffff0" because negative 
     * numbers are represented as: 
     * max_unsigned_value + 1 - abs(negative_num) */
    and     rsp, -16
    call    main

    /* syscall param 1 = rax (ret value of main) */
    mov     rdi, rax
    mov     rax, 60 /* SYS_exit */
    syscall

    ret /* should never be reached, but if the OS somehow fails to kill us, it will cause a segmentation fault */
