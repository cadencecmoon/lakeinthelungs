/*  ----------------------------------------------------------------------------------
 *  |    0    |    1    |    2    |    3    |    4     |    5    |    6    |    7    |
 *  ----------------------------------------------------------------------------------
 *  |   0x0   |   0x4   |   0x8   |   0xc   |   0x10   |   0x14  |   0x18  |   0x1c  |
 *  ----------------------------------------------------------------------------------
 *  | fc_mxcsr|fc_x87_cw|        R12        |         R13        |        R14        |
 *  ----------------------------------------------------------------------------------
 *  ----------------------------------------------------------------------------------
 *  |    8    |    9    |   10    |   11    |    12    |    13   |    14   |    15   |
 *  ----------------------------------------------------------------------------------
 *  |   0x20  |   0x24  |   0x28  |  0x2c   |   0x30   |   0x34  |   0x38  |   0x3c  |
 *  ----------------------------------------------------------------------------------
 *  |        R15        |        RBX        |         RBP        |        RIP        |
 *  ----------------------------------------------------------------------------------
 *  ----------------------------------------------------------------------------------
 *  |    16   |   17    |                                                            |
 *  ----------------------------------------------------------------------------------
 *  |   0x40  |   0x44  |                                                            |
 *  ----------------------------------------------------------------------------------
 *  |        EXIT       |                                                            |
 *  ----------------------------------------------------------------------------------
 */

.file "make_fcontext_amd64_sysv_elf.s"
.text
.globl make_fcontext
.type make_fcontext,@function
.align 16

make_fcontext:
    /* first arg of make_fcontext() == top of context-stack */
    movq    %rdi, %rax

    /* shift address in %rax to lower 16 byte boundary */
    andq    $-16, %rax

    /* reserve space for context-data on context-stack 
     * size for fc_mxcsr .. RIP + return-address for context-function
     * on context-function entry: (%rsp -0x8) % 16 == 0 */
    leaq    -0x48(%rax), %rax

    /* third arg of make_fcontext() == address of context-function */
    movq    %rdx, 0x38(%rax)

    /* save MMX control-word and status-word */
    stmxcsr (%rax)

    /* save x87 control-word */
    fnstcw  0x4(%rax)

    /* compute abs address of label finish */
    leaq    finish(%rip), %rcx

    /* save address of finish as return-address for context-function
     * will be entered after context-function returns */
    movq    %rcx, 0x40(%rax)

    /* return pointer to context-data */
    ret

finish:
    /* exit code is zero */
    xorq    %rdi, %rdi

    /* exit application */
    movq    $231, %rax /* SYS_exit_group */
    syscall
    hlt
.size make_fcontext,.-make_fcontext
/* mark that we don't need executable stack */
.section .note.GNU-stack,"",%progbits
