all: posix_shm posix_shm_dyn

posix_shm: posix_shm.c
	gcc -Wall posix_shm.c -o posix_shm -D_GNU_SOURCE -DALLOW_CLEANUP -std=c99 -lrt

posix_shm_dyn: posix_shm.c
	gcc -Wall posix_shm.c -o posix_shm_dyn -D_GNU_SOURCE -DDYN_SEG_SIZE -DALLOW_CLEANUP -std=c99 -lrt

clean: posix_shm posix_shm_dyn
	rm ./posix_shm
	rm ./posix_shm_dyn
