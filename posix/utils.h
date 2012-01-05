#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>

#define cast(x) (x)
#define printfln(fmt, ...) printf(fmt"\n", ##__VA_ARGS__)
#define print(str) printf("%s", str)
#define println(str) puts(str)

#define fatal(msg) {                                                                               \
  fprintf(stderr, "%s, %d Fatal Error: %s\n", __FILE__, __LINE__, msg);                            \
  exit(EXIT_FAILURE);                                                                              \
}

#define assertv(a, b, ...) if((a) == (b)) {                                                        \
  int __errno = errno;                                                                             \
  fprintf(                                                                                         \
    stderr, "%s, %d Fatal error "__VA_ARGS__": %s, %d\n",                                          \
    __FILE__, __LINE__,                                                                            \
    strerror(errno),                                                                               \
    __errno                                                                                        \
  );                                                                                               \
  exit(EXIT_FAILURE);                                                                              \
}

#define assertz(expr, ...) if(expr) {                                                              \
  int __errno = errno;                                                                             \
  fprintf(                                                                                         \
    stderr, "%s, %d Fatal error "__VA_ARGS__": %s, %d\n",                                          \
    __FILE__, __LINE__,                                                                            \
    strerror(errno),                                                                               \
    __errno                                                                                        \
  );                                                                                               \
  exit(EXIT_FAILURE);                                                                              \
}

#define assertnz(expr, ...) if( !(expr) ) {                                                        \
  int __errno = errno;                                                                             \
  fprintf(                                                                                         \
    stderr, "%s, %d Fatal error "__VA_ARGS__": %s, %d\n",                                          \
    __FILE__, __LINE__,                                                                            \
    strerror(errno),                                                                               \
    __errno                                                                                        \
  );                                                                                               \
  exit(EXIT_FAILURE);                                                                              \
}

#define asserts(expr, ...) if( (expr) == -1) {                                                     \
  int __errno = errno;                                                                             \
  fprintf(                                                                                         \
    stderr, "%s, %d Fatal error "__VA_ARGS__": %s, %d\n",                                          \
    __FILE__, __LINE__,                                                                            \
    strerror(errno),                                                                               \
    __errno                                                                                        \
  );                                                                                               \
  exit(EXIT_FAILURE);                                                                              \
}

typedef unsigned int   uint  ;
typedef unsigned long  ulong ;
typedef unsigned short ushort;
typedef unsigned char  ubyte ;
typedef          char   byte ;
typedef char*         string ;

#endif /* __UTILS_H__ */
