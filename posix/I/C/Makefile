all: posix_sem_client posix_sem_server

posix_sem_client: posix_sem_client.c
	gcc -Wall posix_sem_client.c -o posix_sem_client -D_GNU_SOURCE -std=c99 -pthread

posix_sem_server: posix_sem_server.c
	gcc -Wall posix_sem_server.c -o posix_sem_server -D_GNU_SOURCE -std=c99 -pthread

clean: posix_sem_client posix_sem_server
	rm ./posix_sem_client
	rm ./posix_sem_server
