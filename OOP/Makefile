EXECS= main main2 main3 main4 main5 main6 main7

all:
	make one; make two; make three; make four; make six; make seven; 

git:
	git add -A; git commit; git push;

pull:
	git pull
one:
	mpicxx mainoop.cc -o main
	mpirun -n 4 main -s 1000 -f 1 -i 100

two:
	mpicxx mainoop.cc -o main2
	mpirun -n 4 main2 -s 1000 -f 2 -i 100
three:
	mpicxx mainoop.cc -o main3
	mpirun -n 4 main3 -s 1000 -f 3 -i 100
four:
	mpicxx mainoop.cc -o main4
	mpirun -n 4 main4 -s 1000 -f 4 -i 100
five:
	mpicxx mainoop.cc -o main5
	mpirun -n 15 main5 -s 1000 -f 5 -i 10000
six:
	mpicxx mainoop.cc -o main6
	mpirun -n 4 main6 -s 1000 -f 6 -i 100
seven:
	mpicxx mainoop.cc -o main7
	mpirun -n 4 main7 -s 1000 -f 7 -i 100

clean:
	rm ${EXECS}
