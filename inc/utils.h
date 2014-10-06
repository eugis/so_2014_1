#ifndef __UTILS_H__
#define __UTILS_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define CONCAT1(x) x ## 1
#define CONCAT2(x) CONCAT1(x)
#define IS_EMPTY(x) CONCAT2(x) == 1


#define abort(...) ( (printf(__VA_ARGS__), exit(1), 1 ) )

#define debug(...) ((void) (printf(__VA_ARGS__), fflush(0)))

void check(int condition, char *fmt, ...);

void decdump(void *memory, size_t length);

int streq(char *s1, char *s2);

char *filepath(char *path);
char *filename(char *path);

#endif
