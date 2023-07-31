EXECS= main 

all:
	main; 

git:
	git add -A; git commit; git push;

pull:
	git pull

main:
	mpicxx main.cc -o main
	mpirun -n 2 main 
clean:
	rm ${EXECS}
