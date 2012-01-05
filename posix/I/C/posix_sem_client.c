#include "../../utils.h"
#include <fcntl.h>
#include <semaphore.h>

/* returns the executable path, caller must free() the result */
char* getExecPath(const string o_exec);

int main(int o_argc, const string const* o_argv) { 
  const string SEM_NAME = "/posix_sem";

  /* Try to obtain an id of the semaphore. */
  sem_t* _sd = sem_open(SEM_NAME, 0);
  assertv(_sd, SEM_FAILED);
    
  /* lock the semaphore */
  assertz( sem_wait(_sd) );
  
  /* enter critical section */    
  for(size_t i = 0; i < 5; i++) {
    printfln( "Hello!, %ld", cast(ulong)getpid() );
    sleep(1);
  }
  println("");
      
  /* unlock the semaphore */
  assertz( sem_post(_sd) );
      
  exit(EXIT_SUCCESS);
}
