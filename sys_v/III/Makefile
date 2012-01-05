all: sys_v_shm sys_v_shm_dyn

sys_v_shm: sys_v_shm.c
	gcc -Wall ../utils.o sys_v_shm.c -o sys_v_shm -DALLOW_CLEANUP -D_GNU_SOURCE -std=c99

sys_v_shm_dyn: sys_v_shm.c
	gcc -Wall ../utils.o sys_v_shm.c -o sys_v_shm_dyn -D_GNU_SOURCE -DALLOW_CLEANUP -DDYN_SEG_SIZE -std=c99

clean: sys_v_shm sys_v_shm_dyn
	rm ./sys_v_shm
	rm ./sys_v_shm_dyn
