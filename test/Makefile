EXECS=mul_arr simple notsimple arr_mul
MPICC?=/data/cmssw/el8_amd64_gcc11/cms/cmssw/CMSSW_13_0_8/external/el8_amd64_gcc11/bin/mpicc
#MPICC?=mpicc
mpiCuda: 
	${MPICC} -o mpiCuda mpiCUDA.cu
	python3 run.py mpiCuda

simple: 
	${MPICC} -o simple simple.c
	python3 run.py simple
notsimple: 
	${MPICC} -o notsimple notsimple.c
	python3 run.py notsimple
mul_arr: 
	${MPICC} -o mul_arr mul_arr.c
	python3 run.py mul_arr

arr_mul: 
	${MPICC} -o arr_mul arr_mul.c
	python3 run.py arr_mul
mpi_sendRecv: 
	${MPICC} -o mpi_sendRecv mpi_sendRecv.c
	python3 run.py mpi_sendRecv
win: 
	${MPICC} -o win win.c
	python3 run.py win
window: 
	${MPICC} -o window window.c
	python3 run.py window


main:
	${MPICC} -o main main_win.c
	python3 run.py main

simple_onesided:
	${MPICC} -o simple_onesided simple_onesided.c
	python3 run.py simple_onesided

clean:
	rm ${EXECS}

