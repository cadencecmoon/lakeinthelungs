#!/bin/bash

echo "#ifndef _AMW_POSIX_SYSCALLS_TABLE_H" >> $2
echo "#define _AMW_POSIX_SYSCALLS_TABLE_H" >> $2
cat $1 | awk '/^[0-9]/ {nr=$1; abi=$2; name=$3; entry=$4; printf("#define SYS_%s_%s\t%d\t/: %s :/\n", name, abi, nr, entry); }' | sort | sed -e 's/_common//g' | sed -e 's/_64//g' | sed -e 's@:@*@g' >> $2
echo "#endif" >> $2

