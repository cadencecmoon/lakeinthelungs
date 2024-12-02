/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/amw.h>

#include "posix_syscalls_table.h"

extern void *syscall3(void *sysno, void *arg1, void *arg2, void *arg3);

int a_moonlit_walk(struct amw_engine *amw)
{
    (void)amw;

    syscall3((void*)SYS_write, (void*)(intptr_t)1, (void*)"uwuwu wuwu\n", (void*)(uintptr_t)10u);
    return 0;
}

void amw_default_hints(struct amw_engine_hints *hints)
{
    (void)hints; 
}
