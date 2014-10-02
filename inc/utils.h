#ifndef __UTILS_H__
#define __UTILS_H__

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define abort(...) ( (printf(__VA_ARGS__), exit(1), 1 ) )
#define check(expr, ...)  ((void) (((expr) >= 0)    || abort(__VA_ARGS__)) )
#define checkp(expr, ...) ((void) (((expr) != NULL) || abort(__VA_ARGS__)) )

int streq(char *s1, char *s2);

#endif
