#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define DEBUG_int(x) do { printf("%s = %d\n", #x, (x)); } while (0);

#endif