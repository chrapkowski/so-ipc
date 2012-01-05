#ifndef __SYS_V_SEMUN_H__
#define __SYS_V_SEMUN_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#if defined(_SEM_SEMUN_UNDEFINED)
union semun {
  int              val  ;
  struct semid_ds* buf  ;
  unsigned short*  array;
  #if defined(__linux__)
  struct seminfo*  __buf;
  #endif
};
#endif

#endif // __SYS_V_SEMUN_H__
