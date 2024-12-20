if host_machine.cpu_family() == 'x86' or host_machine.cpu_family() == 'x86_64'
    subdir('x86')
elif host_machine.cpu_family() == 'arm' or host_machine.cpu_family() == 'aarch64'
    subdir('arm')
elif host_machine.cpu_family() == 'riscv32' or host_machine.cpu_family() == 'riscv64'
    subdir('riscv')
else
    error('CPU architecture \'' + host_machine.cpu_family() + '\' is not yet supported.')
endif
