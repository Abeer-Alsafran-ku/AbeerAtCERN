EXECS=split allreduce_dev reduce all_gather gat_scat_avg compare_bcast my_bcast mpi_walking mpi_probe mpi_hello_world mpi_ring mpi_ping_pong mpi_send_recv mpi_check 


MPICC?=/data/cmssw/el8_amd64_gcc11/cms/cmssw/CMSSW_13_0_8/external/el8_amd64_gcc11/bin/mpicc
MPICXX?=/data/cmssw/el8_amd64_gcc11/cms/cmssw/CMSSW_13_0_8/external/el8_amd64_gcc11/bin/mpicxx
#MPICC?=mpicc


all:split allreduce_dev reduce all_gather gat_scat_avg compare_bcast my_bcast mpi_walking mpi_probe mpi_check mpi_hello mpi_send mpi_ping mpi_ring

mpi_hello: 
	${MPICC} -o mpi_hello_world mpi_hello_world.c

mpi_send:
	${MPICC} -o mpi_send_recv mpi_send_recv.c
	python3 run.py mpi_send_recv
mpi_sendrecv:
	${MPICC} -o mpi_sendrecv mpi_sendrecv.c
	python3 run.py mpi_sendrecv


mpi_ping:
	${MPICC} -o mpi_ping_pong mpi_ping_pong.c
	python3 run.py mpi_ping_pong

mpi_ring:
	${MPICC} -o mpi_ring mpi_ring.c
	python3 run.py mpi_ring

mpi_check:
	${MPICC} -o mpi_check_status mpi_check_status.c
	python3 run.py mpi_check_status

mpi_probe:
	${MPICC} -o mpi_probe mpi_probe.c
	python3 run.py mpi_probe

mpi_walking:
	${MPICXX} -o mpi_walking mpi_walking.cc
	python3 run.py mpi_walking

my_bcast:
	${MPICC} -o my_bcast my_bcast.c
	python3 run.py my_bcast

compare_bcast:
	${MPICC} -o compare_bcast compare_bcast.c
	python3 run.py compare_bcast

gat_scat_avg:
	${MPICC} -o gat_scat_avg gat_scat_avg.c
	python3 run.py gat_scat_avg

all_gather:
	${MPICC} -o all_gather all_gather.c
	python3 run.py all_gather

reduce:
	${MPICC} -o reduce reduce.c
	python3 run.py reduce

allreduce_dev:
	${MPICC} -o allreduce_dev allreduce_dev.c -lm
	python3 run.py allreduce_dev

split:
	${MPICC} -o split split.c
	python3 run.py split
win:
	${MPICC} -o mpi_win mpi_win.c
	python3 run.py mpi_win

clean:
	rm ${EXECS}

