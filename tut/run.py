#!/usr/bin/python
import sys
import os
import subprocess

# Enter runnable programs here, keyed on the program executable name and followed
# by a tuple of the tutorial name and the default number of nodes
programs = {
    # From the mpi-hello-world tutorial
    'mpi_hello_world': ('mpi-hello-world', 4),

    # From mpi-send-and-receive tutorial
    'mpi_send_recv': ('', 2),
    'mpi_sendrecv': ('', 2),
    'mpi_ping_pong': ('', 2),
    'mpi_ring': ('', 5),

    # From the dynamic-receiving-with-mpi-probe-and-mpi-status tutorial
    'mpi_check_status': ('', 2),
    'mpi_probe': ('', 2),

    # From the point-to-point-communication-application-random-walk tutorial
    
                    # no. proc #domain  #max walk size  #walkers/proc
    'mpi_walking': ('', 2,      ['100', '500',          '40']),

    # From the mpi-broadcast-and-collective-communication tutorial
    'my_bcast': ('', 8),
    #'compare_bcast': ('', 2, ['100', '2']),
    'compare_bcast': ('', 16, ['100000', '10']),

    # From the mpi-scatter-gather-and-allgather tutorial
    'gat_scat_avg': ('', 4, ['100']),
    'all_gather': ('', 6, ['100']),

    # From the performing-parallel-rank-with-mpi tutorial
    'random_rank': ('', 4, ['100']),

    # From the mpi-reduce-and-allreduce tutorial
    'reduce': ('', 4, ['20']),
    'allreduce_dev': ('', 4, ['100']),

    # From the groups-and-communicators tutorial
    'split': ('', 8),
    'mpi_win': ('', 2),
    'groups': ('', 16)
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
