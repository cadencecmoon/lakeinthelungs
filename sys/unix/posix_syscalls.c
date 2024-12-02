/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/common/defines.h>

#include <sys/types.h>
#include <sys/cdefs.h>

/* header generated in meson, architecture specific */
#include "posix_syscalls_table.h"

extern void *syscall0(void *sysno);
extern void *syscall1(void *sysno, void *arg1);
extern void *syscall2(void *sysno, void *arg1, void *arg2);
extern void *syscall3(void *sysno, void *arg1, void *arg2, void *arg3);
extern void *syscall4(void *sysno, void *arg1, void *arg2, void *arg3, void *arg4);
extern void *syscall5(void *sysno, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5);
extern void *syscall6(void *sysno, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6);
