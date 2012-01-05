all: sys_v_sem_client sys_v_sem_server

sys_v_sem_client: sys_v_sem_client.c
	gcc -Wall ../../utils.o sys_v_sem_client.c -o sys_v_sem_client -D_GNU_SOURCE -std=c99

sys_v_sem_server: sys_v_sem_server.c
	gcc -Wall ../../utils.o sys_v_sem_server.c -o sys_v_sem_server -D_GNU_SOURCE -std=c99

clean: sys_v_sem_client sys_v_sem_server
	rm ./sys_v_sem_client
	rm ./sys_v_sem_server
