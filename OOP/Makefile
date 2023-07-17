
EXECS: main main2
all: main main2 

git:
	git add -A; git commit; git push;

pull:
	git pull

one:
	mpicxx mainoop.cc -o main
	mpirun -n 3 main -s 10 -f 1 -i 2

two:
	mpicxx mainoop.cc -o main2
	mpirun -n 3 main2 -s 10 -f 1 -i 2

clean:
	rm ${EXECS}
