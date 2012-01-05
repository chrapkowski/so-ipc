#include "../utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <limits.h>
#include <semaphore.h>

typedef struct mydata_t {
  size_t len ; /* number of arguments */
  #ifdef DYN_SEG_SIZE
  char   vals[0]; /* buffer */
  #else
  char   vals[LINE_MAX]; /* buffer; */
  #endif /* DYN_SEG_SIZE */
} mydata_t, *MyData;

int main(int o_argc, const string const* o_argv) {

  MyData _data;
  int    _shmd;
  size_t _off ;
  const string SEM_NAME = "/posix_sem";
  const string SHM_NAME = "/posix_shm";
  
  /* Nowaday, memory is so cheap that I just do not care about the unused memory.
   * If is it too waste, then just compile with DYN_SEG_SIZE switch.
  **/
  #ifndef DYN_SEG_SIZE
  const off_t SEG_SIZE = sizeof(mydata_t);
  #else
  off_t _segSz = sizeof(size_t);
  for(size_t i = 1; i < o_argc; i++) {
    _segSz += strlen(o_argv[i]) + 1;
  }
  #endif /* DYN_SEG_SIZE */
  
  /* open/create semaphore and lock */
  sem_t* _sem = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
  assertv(_sem, SEM_FAILED);
  assertz( sem_wait(_sem) );
  
  /* Critical section: */
  
  /* there is no arguments so print shared memory object content */
  if(o_argc == 1) {
  
    /* obtain the descriptor of shared memory object */
    asserts( _shmd = shm_open(SHM_NAME, O_RDONLY, S_IRUSR | S_IWUSR) );
    
    /* map shared memory object into a virtual address */
    #ifdef DYN_SEG_SIZE
    struct stat _buf;
    assertz( fstat(_shmd, &_buf) );
    assertv(_data = mmap(NULL, _segSz = _buf.st_size, PROT_READ, MAP_SHARED, _shmd, 0), MAP_FAILED);
    #else
    assertv(_data = mmap(NULL, SEG_SIZE, PROT_READ, MAP_SHARED, _shmd, 0), MAP_FAILED);
    #endif /* DYN_SEG_SIZE */
    
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
  
    #ifdef ALLOW_CLEANUP
    int _ret;
    /* if shared memory object already exist obtain its id and destroy, otherwise do nothing */
    if( o_argc == 2 && !strcmp(o_argv[1], "cleanup") ) {
      _ret = shm_unlink(SHM_NAME);  
      if(_ret == -1 && errno != ENOENT) asserts(_ret);
      
      /* destroy the semaphore before exit */
      _ret = sem_unlink(SEM_NAME);
      if(_ret == -1 && errno != ENOENT) asserts(_ret);
      exit(EXIT_SUCCESS);
    } 
    #endif /* ALLOW_CLEANUP */
  
    /* use existing shared memory object or create the new one */
    asserts( _shmd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR) );
    
    /* allocate a space for the maximum line length */
    #ifdef DYN_SEG_SIZE
    assertz( ftruncate(_shmd, _segSz) );
    #else
    assertz( ftruncate(_shmd, SEG_SIZE) );
    #endif /* DYN_SEG_SIZE */
    
    /* map shared memory object into virtual address */
    #ifdef DYN_SEG_SIZE
    assertv(_data = mmap(NULL, _segSz, PROT_READ | PROT_WRITE, MAP_SHARED, _shmd, 0), MAP_FAILED);
    #else
    assertv(_data = mmap(NULL, SEG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _shmd, 0), MAP_FAILED);
    #endif /* DYN_SEG_SIZE */

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
  #ifdef DYN_SEG_SIZE
  assertz( munmap(_data, _segSz) );
  #else
  assertz( munmap(_data, SEG_SIZE) );
  #endif /* DYN_SEG_SIZE */
  _data = NULL;
 
  /* close the unused descriptor of shared memory object */ 
  assertz( close(_shmd) );
  
  /* unlock the semaphore */
  assertz( sem_post(_sem) );
  exit(EXIT_SUCCESS);
}
