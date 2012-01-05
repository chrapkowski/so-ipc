#include "../../utils.h"
#include "../../sys_v_semun.h"
#include <fcntl.h>
#include <sys/wait.h>

/* returns the executable path, caller must free() the result */
char* getExecPath(const string o_exec);

/* prints the usage notes and terminates the process */
void printUsage(const string o_exec);

int main(int o_argc, const string const* o_argv) {
  union  semun   _arg;
  struct sembuf  _buf;
  
  _buf.sem_num = 0;
  _buf.sem_flg = 0;

  if(o_argc != 2) printUsage(*o_argv);
  
  string _path = getExecPath("sys_v_sem");
  key_t _key = ftok(_path, 'x');
  asserts(_key, "ftok");
  free(_path);
  _path = NULL;
  
  /* start server */
  if( !strcmp(o_argv[1], "start") ) {
  
    /* Try to create a set of semaphores. */
    int _semid = semget(_key, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    
    /* Server is already running. */
    if(_semid == -1 && errno == EEXIST) exit(EXIT_SUCCESS);
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
  }
  else
  
  /* stop server */
  if( !strcmp(o_argv[1], "stop") ) {
  
    /* Try to obtain an id of the set of semaphores. */
    int _semid = semget(_key, 1, S_IRUSR | S_IWUSR);
    
    /* If server is not running then exit. */
    if(_semid == -1 && errno == ENOENT) exit(EXIT_SUCCESS);
    
    /* Destroy the set of semaphores. */
    asserts( semctl(_semid, 0, IPC_RMID, _arg) );
  }
  
  else printUsage(*o_argv);
  
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

void printUsage(const string o_exec) {
  printfln(
    "Usage:\n"
    "%s start\n"
    "%s stop",
    o_exec, o_exec
  );
  exit(EXIT_SUCCESS);
}
