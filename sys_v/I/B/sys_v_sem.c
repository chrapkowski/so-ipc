#include "../../utils.h"
#include "../../sys_v_semun.h"
#include <fcntl.h>
#include <sys/wait.h>

int main(int o_argc, const string const* o_argv) {
  union  semun  _arg   ;
  struct sembuf _buf   ;
  int           _status;
  pid_t         _pid   ;
  
  if(o_argc != 2) {
    printfln("Usage: %s <number_of_processes>", o_argv[0]);
    exit(EXIT_SUCCESS);
  }
  
  size_t _nproc = atoi(o_argv[1]);
  if(_nproc <= 0) fatal("Number of the processes must be greater than zero.");
  
  
  _buf.sem_num = 0;
  _buf.sem_flg = 0;

  /* Try to create a set of semaphores. */
  int _semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  asserts(_semid);
  
  /* init semaphore */ {
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
  
  /* run child processes */
  for(size_t i = 0; i < _nproc; i++) {
    _pid = fork();
    asserts(_pid);
    
    /* child process */
    if(_pid == 0) {
    
      /* lock the semaphore */
      _buf.sem_op = -1;
      asserts( semop(_semid, &_buf, 1) );
      
      for(size_t i = 0; i < 5; i++) {
        printfln( "Hello!, %ld", cast(ulong)getpid() );
        sleep(1);
      }
      println("");
      
      /* unlock the semaphore */
      _buf.sem_op = 1;
      asserts( semop(_semid, &_buf, 1) );
      
      exit(EXIT_SUCCESS);
    }
    /* parent, run the rest of the processes */
  }
  
  /* parent process */
  for(size_t i = 0; i < _nproc; i++) {
    asserts( wait(&_status) ); /* wait for all children, order is uncertain */
  }
  
  /* Destroy the set of semaphores */
  asserts( semctl(_semid, 0, IPC_RMID, _arg) );
  
  exit(EXIT_SUCCESS);
}
