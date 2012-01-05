all: sys_v_sem sys_v_sem_auto_rm

sys_v_sem: sys_v_sem.c
	gcc -Wall ../utils.o sys_v_sem.c -o sys_v_sem -D_GNU_SOURCE -std=c99

sys_v_sem_auto_rm: sys_v_sem.c
	gcc -Wall ../utils.o sys_v_sem.c -o sys_v_sem_auto_rm -D_GNU_SOURCE -DAUTO_RM_SEM -std=c99

clean: sys_v_sem sys_v_sem_auto_rm
	rm ./sys_v_sem
	rm ./sys_v_sem_auto_rm
