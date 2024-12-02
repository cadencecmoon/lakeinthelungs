#!/bin/bash

echo "#ifndef _AMW_POSIX_SYSCALLS_TABLE_H" >> $2
echo "#define _AMW_POSIX_SYSCALLS_TABLE_H" >> $2
cat $1 | awk '/^[0-9]/ {nr=$1; abi=$2; name=$3; entry=$4; printf("#define SYS_%s\t%d\t/: %s %s :/\n", name, nr, abi, entry); }' | sort | sed -e 's@:@*@g' >> $2
echo "#endif" >> $2
