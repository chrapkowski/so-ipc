#include "utils.h"

char* getExecPath(const string o_exec) {
  char _path[PATH_MAX];
  assertnz( getcwd(_path, PATH_MAX) );
  
  const string _filename = basename(o_exec);
  
  strcat(_path, "/");
  strcat(_path, _filename);

  string _res = malloc(strlen(_path) + 1);  
  strcpy(_res, _path);

  return _res;
}
