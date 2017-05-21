a.out:
	gcc -c -pthread threadpool.c -g
	gcc -pthread -o example example.c -g threadpool.o
