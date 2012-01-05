#include "../../utils.h"
#include <fcntl.h>
#include <semaphore.h>

/* prints the usage notes and terminates the process */
void printUsage(const string o_exec);

int main(int o_argc, const string const* o_argv) {

  if(o_argc != 2) printUsage(*o_argv);
  
  const string SEM_NAME = "/posix_sem";
  
  /* start server */
  if( !strcmp(o_argv[1], "start") ) {
  
    /* Try to create a semaphore. */
    sem_t* _sd = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    
    /* Server is already running. */
    if(_sd == SEM_FAILED && errno == EEXIST) exit(EXIT_SUCCESS);
    assertv(_sd, SEM_FAILED);
    
    assertz( sem_close(_sd) );
  }
  else
  
  /* stop server */
  if( !strcmp(o_argv[1], "stop") ) {
  
    /* Try to obtain an id of the set of semaphores. */
    int _ret = sem_unlink(SEM_NAME);
    
    /* If server is not running then exit. */
    if(_ret == -1 && errno == ENOENT) exit(EXIT_SUCCESS);
    assertz(_ret);
  }
  
  else printUsage(*o_argv);
  
  exit(EXIT_SUCCESS);
}

void printUsage(const string o_exec) {
  printfln(
    "Usage:\n"
    "%s start\n"
    "%s stop",
    o_exec, o_exec
  );
  exit(EXIT_SUCCESS);
}
