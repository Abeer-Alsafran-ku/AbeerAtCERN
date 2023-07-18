
EXECS= cleanMPI1 cleanMPI2
all: 
	one two 

one:
	mpicxx cleanMPI.cc -o cleanMPI1
	mpirun -n 3 cleanMPI1 -s 10 -r 1 -n 2

two:
	mpicxx cleanMPI.cc -o cleanMPI2
	mpirun -n 3 cleanMPI2 -s 10 -r 1 -n 2


clean:
	rm ${EXECS}
