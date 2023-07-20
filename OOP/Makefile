EXECS= main main2 main3 main4 main5 main6

all:
	one; two; three; four; five; six; 

git:
	git add -A; git commit; git push;

pull:
	git pull

one:
	mpicxx mainoop.cc -o main
	mpirun -n 3 main -s 10 -f 1 -i 2

two:
	mpicxx mainoop.cc -o main2
	mpirun -n 3 main2 -s 10 -f 2 -i 2
three:
	mpicxx mainoop.cc -o main3
	mpirun -n 3 main3 -s 10 -f 3 -i 2
four:
	mpicxx mainoop.cc -o main4
	mpirun -n 3 main4 -s 10 -f 4 -i 2
five:
	mpicxx mainoop.cc -o main5
	mpirun -n 3 main5 -s 10 -f 5 -i 2
six:
	mpicxx mainoop.cc -o main6
	mpirun -n 3 main6 -s 10 -f 6 -i 2 

clean:
	rm ${EXECS}
