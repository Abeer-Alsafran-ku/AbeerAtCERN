EXECS= main main2 main3 main4 main5 main6 main7

all:
	make one; make two; make three; make four; make six; make seven; 

git:
	git add -A; git commit; git push;

pull:
	git pull

one:
	mpicxx mainoop.cc -o main
	mpirun -n 8 main -s 1000000 -f 1 -i 10000

two:
	mpicxx mainoop.cc -o main2
	mpirun -n 8 main2 -s 1000000 -f 2 -i 10000
three:
	mpicxx mainoop.cc -o main3
	mpirun -n 8 main3 -s 1000000 -f 3 -i 10000
four:
	mpicxx mainoop.cc -o main4
	mpirun -n 8 main4 -s 1000000 -f 4 -i 10000
five:
	mpicxx mainoop.cc -o main5
	mpirun -n 15 main5 -s 1000 -f 5 -i 10000
six:
	mpicxx mainoop.cc -o main6
	mpirun -n 8 main6 -s 1000000 -f 6 -i 10000
seven:
	mpicxx mainoop.cc -o main7
	mpirun -n 8 main7 -s 1000000 -f 7 -i 10000

clean:
	rm ${EXECS}
