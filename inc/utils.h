#ifndef __UTILS_H__
#define __UTILS_H__

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define abort(...) ( (printf(__VA_ARGS__), exit(1), 1 ) )
#define check(expr, ...)  ((void) ((expr) || abort(__VA_ARGS__)) )

#define debug(...) ((void) (printf(__VA_ARGS__), fflush(0)))

int streq(char *s1, char *s2);

char *filepath(char *path);
char *filename(char *path);

#endif
