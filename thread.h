#include <string.h>
#include "calc.h"
struct connInfo{
  int clientfd;
  const char *webroot;
  struct Calc * myCalc;
};
