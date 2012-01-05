#include "../../utils.h"
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int o_argc, const string const* o_argv) {
  
  int    _status;
  pid_t  _pid   ;
  sem_t* _sem   ;
  
  if(o_argc != 2) {
    printfln("Usage: %s <number_of_processes>", o_argv[0]);
    exit(EXIT_SUCCESS);
  }
  
  size_t _nproc = atoi(o_argv[1]);
  if(_nproc <= 0) fatal("Number of the processes must be greater than zero.");
  
  const string SEM_NAME = "/posix_sem";
  
  /* Create a semaphore */
  _sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  assertv(_sem, SEM_FAILED);
  
  /* run child processes */
  for(size_t i = 0; i < _nproc; i++) {
    _pid = fork();
    asserts(_pid);
    
    /* child process */
    if(_pid == 0) {
    
      /* lock the semaphore */
      assertz( sem_wait(_sem) );
      
      /* enter critical section */
      for(size_t i = 0; i < 5; i++) {
        printfln( "Hello!, %ld", cast(ulong)getpid() );
        sleep(1);
      }
      println("");
      
      /* unlock the semaphore */
      assertz( sem_post(_sem) );
      exit(EXIT_SUCCESS);
    }
    /* parent, run the rest of the processes */
  }
  
  /* parent process */
  for(size_t i = 0; i < _nproc; i++) {
    asserts( wait(&_status) ); /* wait for all children, order is uncertain */
  }
  
  /* Destroy the semaphore */
  assertz( sem_close(_sem) );
  assertz( sem_unlink(SEM_NAME) );
  
  exit(EXIT_SUCCESS);
}
