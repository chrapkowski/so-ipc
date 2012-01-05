#include "../../utils.h"
#include "../../sys_v_semun.h"
#include <fcntl.h>
#include <sys/wait.h>

/* returns the executable path, caller must free() the result */
char* getExecPath(const string o_exec);

int main(int o_argc, const string const* o_argv) {
  struct sembuf _buf;
    
  _buf.sem_num = 0;
  _buf.sem_flg = 0;
  
  string _path = getExecPath("sys_v_sem");
  key_t _key = ftok(_path, 'x');
  asserts(_key, "ftok");
  free(_path);
  _path = NULL;

  /* Try to obtain an id of the set of semaphores. */
  int _semid = semget(_key, 1, S_IRUSR | S_IWUSR);
  asserts(_semid);
    
  /* lock the semaphore */
  _buf.sem_op = -1;
  asserts( semop(_semid, &_buf, 1) );
  
  /* enter critical section */    
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
