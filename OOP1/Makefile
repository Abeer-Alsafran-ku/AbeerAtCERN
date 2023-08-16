EXECS= main main2 main3 main4 main5 main6 main7

all:
	make one; make two; make three; make four; make five; make six; make seven; 

oneside:
	make five; make seven;
git:
	git add -A; git commit; git push;

pull:
	git pull
one:
	mpicxx mainoop.cc -o main
	mpirun -n 6 main -s 100 -f 1 -i 10
two:
	mpicxx mainoop.cc -o main2
	mpirun -n 6 main2 -s 100 -f 2 -i 10
three:
	mpicxx mainoop.cc -o main3
	mpirun -n 6 main3 -s 100 -f 3 -i 10
four:
	mpicxx mainoop.cc -o main4
	mpirun -n 6 main4 -s 100 -f 4 -i 10
#master
five:
	mpicxx mainoop.cc -o main5
	mpirun -n 6 main5 -s 100 -f 5 -i 10 
six:
	mpicxx mainoop.cc -o main6
	mpirun -n 6 main6 -s 100 -f 6 -i 10
#worker
seven:
	mpicxx mainoop.cc -o main7
	mpirun -n 6 main7 -s 100 -f 7 -i 10 
clean:
	rm ${EXECS}
