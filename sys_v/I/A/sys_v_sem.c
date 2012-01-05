#include "../../utils.h"
#include "../../sys_v_semun.h"
#include <libgen.h>
#include <fcntl.h>

/* returns the executable path, caller must free() the result */
char* getExecPath(const string o_exec);

int main(int o_argc, const string const* o_argv) {
  string _path = getExecPath(*o_argv);
  key_t _key = ftok(_path, 'x');
  asserts(_key, "ftok");
  free(_path);
  _path = NULL;

  union  semun  _arg;
  struct sembuf _buf;
  
  _buf.sem_num = 0;
  _buf.sem_flg = 0;

  /* Try to create a set of semaphores*/
  int _semid = semget(_key, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  
  if(_semid == -1) {
    const int MAX_TRIES = 6;
    
    /* If semget failed, and the set does not exist then exit */
    if(errno != EEXIST) asserts(_semid, "semget");
    
    _semid = semget(_key, 1, S_IRUSR | S_IWUSR);
    asserts(_semid);
    
    struct semid_ds _ds;
    
    _arg.buf = &_ds;
    
    for(size_t i = 0; i < MAX_TRIES; i++) {
      asserts( semctl(_semid, 0, IPC_STAT, _arg) );
      if(_ds.sem_otime != 0) break;
      sleep(5);
    }
    
    if(_ds.sem_otime == 0)
      fatal (
        "The set of semaphores already exists, but it is not initialized.\n"
        "This is a permanent error, and I have given up."
      )
    ;
  }
  
  /* init semaphore */
  else {
  
    /* Note:
     *   Some systems, like Linux, implicitly initializes a set of semaphores by value 0,
     *   but unfortunately some does not. For that reason, this operation is necessary to ensure
     *   a portability.
    **/
    _arg.val = 0;
    asserts( semctl(_semid, 0, SETVAL, _arg) );
    
    /* post semaphore */
    _buf.sem_op = 1;
    asserts( semop(_semid, &_buf, 1) );
  }
  
  /* lock the semaphore */  
  _buf.sem_op = -1;
  asserts( semop(_semid, &_buf, 1) );
  
  /* the critical section */
  for(size_t i = 0; i < 5; i++) {
    printfln( "Hello!, %ld", cast(ulong)getpid() );
    sleep(1);
  }
  println("");
  
  /* Check, if is there any process which wait for the set. If no then destroy the set.
   * Bug:
   *   That approach can cause that some process tries to lock a set which does not already
   *   exist. That is not a big problem because such cases are relativly rare and such process
   *   does not enter the critical section. 
  **/
  #ifdef AUTO_RM_SEM
  int _semncnt =  semctl(_semid, 0, GETNCNT, _arg);
  asserts(_semncnt);
  
  if(_semncnt == 0) {
    asserts( semctl(_semid, 0, IPC_RMID, _arg) );
  }
  else
  #endif /* AUTO_RM_SEM */
  {
    /* unlock the semaphore */
    _buf.sem_op = 1;
    asserts( semop(_semid, &_buf, 1) );
  }

  exit(EXIT_SUCCESS);
}

char* getExecPath(const string o_exec) {
  string _path = get_current_dir_name();
  assertnz(_path);
  
  const string _filename = basename(o_exec);
  _path = realloc(_path, strlen(_path) + strlen(_filename) + 2);
  assertnz(_path);
  
  strcat(_path, "/");
  strcat(_path, _filename);
  
  return _path;
}
