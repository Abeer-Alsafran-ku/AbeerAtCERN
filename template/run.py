#!/usr/bin/python
import sys
import os
import subprocess

# Enter runnable programs here, keyed on the program executable name and followed
# by a tuple of the tutorial name and the default number of nodes
programs = {
    # From the mpi-hello-world tutorial
    'simple': ('', 2),
    'mpi_sendRecv': ('', 4),
    'notsimple': ('', 3),
    'mul_arr': ('', 4, ['70']),
    'arr_mul': ('', 4, ['70']),
    'win': ('', 2),
    'main': ('', 2),
    'simple_onesided': ('', 2),
    'window': ('', 2),
}

program_to_run = sys.argv[1] if len(sys.argv) > 1 else None
if not program_to_run in programs:
    print('Must enter program name to run. Possible programs are: {0}'.format(programs.keys()))
else:
    # Try to compile before running
    with open(os.devnull, 'wb') as devnull:
        subprocess.call(
            [''.format(programs[program_to_run][0])],
            #['cd ./{0} && make'.format(programs[program_to_run][0])],
            stdout=devnull, stderr=subprocess.STDOUT, shell=True)

    mpirun = os.environ.get('MPIRUN', 'mpirun')
    hosts = '' if not os.environ.get('MPI_HOSTS') else '-f {0}'.format(os.environ.get('MPI_HOSTS'))

    sys_call = '{0} -n {1} {2}./{3}{4}'.format(
        mpirun, programs[program_to_run][1], hosts, programs[program_to_run][0], program_to_run)

    if len(programs[program_to_run]) > 2:
        sys_call = '{0} {1}'.format(sys_call, ' '.join(programs[program_to_run][2]))

    print(sys_call)
    subprocess.call([sys_call], shell=True)
