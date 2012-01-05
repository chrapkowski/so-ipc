#include "../utils.h"
#include "../sys_v_semun.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <libgen.h>
#include <limits.h>
#include <fcntl.h>

typedef struct mydata_t {
  size_t len; /* number of arguments */
  #ifdef DYN_SEG_SIZE
  char  vals[0]; /* buffer */
  #else
  char  vals[LINE_MAX]; /* buffer; */
  #endif /* DYN_SEG_SIZE */
} mydata_t, *MyData;

/* returns the executable path, caller must free() the result */
char* getExecPath(const string o_exec);

int main(int o_argc, const string const* o_argv) {
  string _path = getExecPath(*o_argv);
  key_t _key = ftok(_path, 'x');
  asserts(_key, "ftok");
  free(_path);
  _path = NULL;

  union  semun  _arg  ;
  struct sembuf _buf  ;
  int           _shmid;
  MyData        _data ;
  size_t        _off  ;
  
  /* Nowaday, memory is so cheap that I just do not care about the unused memory.
   * If is it too waste, then just compile with DYN_SEG_SIZE switch.
  **/
  #ifndef DYN_SEG_SIZE
  const off_t SEG_SIZE = sizeof(mydata_t);
  #endif /* DYN_SEG_SIZE */
  
  _buf.sem_num = 0;
  _buf.sem_flg = 0;

  /* Try to create a set of semaphores. */
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
  
  /* Critical section: */ 
  
  /* there is no arguments so print shared memory object content */
  if(o_argc == 1) {
  
  
    /* obtain the descriptor of shared memory object */
    asserts( _shmid = shmget(_key, 0, S_IRUSR | S_IWUSR | SHM_RDONLY) );
    
    /* map shared memory object into virtual address space */
    assertv(_data = shmat(_shmid, NULL, 0), cast(void*)-1);

    /* read from shared memory object */
    _off = 0;
    for(size_t i = 0; i < _data->len; i++) {
      print(_data->vals + _off);
      print(" ");
      _off += strlen(_data->vals + _off) + 1;
    }
    
    println("");
  }
  
  /* write arguments in the reveres order into shared memory object */
  else {
  
    #if (defined ALLOW_CLEANUP || defined DYN_SEG_SIZE)
    struct shmid_ds _shmds;
    #endif /* ALLOW_CLEANUP || DYN_SEG_SIZE */
    
    #ifdef ALLOW_CLEANUP
    union semun _semun;
    
    /* if shared memory object already exist obtain its id and destroy, otherwise do nothing */
    if( o_argc == 2 && !strcmp(o_argv[1], "cleanup") ) {
      _shmid = shmget(_key, 0, S_IRUSR | S_IWUSR);  
      if(_shmid == -1) {
        if(errno != ENOENT) asserts(_shmid);
      }
      else asserts( shmctl(_shmid, IPC_RMID, &_shmds) );
      
      /* destroy the semaphore before exit */
      asserts( semctl(_semid, 0, IPC_RMID, _semun) );
      exit(EXIT_SUCCESS);
    } 
    #endif /* ALLOW_CLEANUP */
    
    /* use existing shared memory object or create the new one */
    #ifdef DYN_SEG_SIZE  
    off_t _segSz = sizeof(size_t);
    for(size_t i = 1; i < o_argc; i++) {
      _segSz += strlen(o_argv[i]) + 1;
    }

    /* Try to create a new shared memory object.
     * If such object already exits the destoy it before.
    **/
    _shmid = shmget(_key, _segSz, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL);
    if(_shmid == -1) {
      if(errno == EEXIST) {
        asserts( _shmid = shmget(_key, 0, S_IRUSR | S_IWUSR) );
        asserts( shmctl(_shmid, IPC_RMID, &_shmds) );
        asserts( _shmid = shmget(_key, _segSz, S_IRUSR | S_IWUSR | IPC_CREAT) );
      }
    }
    #else
    asserts( _shmid = shmget(_key, SEG_SIZE, S_IRUSR | S_IWUSR | IPC_CREAT) );
    #endif /* DYN_SEG_SIZE */

    /* map shared memory object into virtual address space */
    assertv(_data = shmat(_shmid, NULL, 0), cast(void*)-1);
    
    /* write into the shared memory object */
    _data->len = o_argc - 1;
    _off = 0;
    for(size_t i = o_argc - 1; i > 0; i--) {
      /* it is safe to use strcpy, because we got enought memory */
      strcpy(_data->vals + _off, o_argv[i]);
      _off += strlen(o_argv[i]) + 1;
    }
  }
   
  /* unmap shared memory object from virtual address space */
  assertz( shmdt(_data) );
  _data = NULL;
  
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
