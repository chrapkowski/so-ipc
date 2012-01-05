#include "../utils.h"
#include <semaphore.h>
#include <fcntl.h>

int main(int o_argc, const string const* o_argv) {
  
  const string SEM_NAME = "/posix_sem";

  /* try to create a semaphore */
  sem_t* _semd = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
  assertv(_semd, SEM_FAILED);  
  
  println("Press any key to continue...");
  getchar();
  
  /* lock the semaphore */  
  assertz( sem_wait(_semd) );
  
  println("Press any key to release the resources...");
  getchar();
  
  /* unlock and close semaphore */
  assertz( sem_post(_semd) );
  assertz( sem_close(_semd) );

  exit(EXIT_SUCCESS);
}
