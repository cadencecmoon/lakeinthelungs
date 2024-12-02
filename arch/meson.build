# Lake in the Lungs
# Copyright (c) 2024 Cadence C. Moon
# The source code is licensed under a standard MIT license.

cpufamily = host_machine.cpu_family()

if cpufamily == 'x86' or cpufamily == 'x86_64'
    subdir('x86')
elif cpufamily == 'arm' or cpufamily == 'aarch64'
    subdir('arm')
elif cpufamily == 'riscv32' or cpufamily == 'riscv64'
    subdir('riscv')
endif
