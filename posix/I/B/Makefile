all: posix_sem posix_sem_server

posix_sem_server:
	$(MAKE) -C ../C
	ln -s ../C/posix_sem_server

posix_sem: posix_sem.c
	gcc -Wall posix_sem.c -o posix_sem -D_GNU_SOURCE -std=c99 -pthread

clean: posix_sem
	rm ./posix_sem
	rm ./posix_sem_server
