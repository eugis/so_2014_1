#include "../inc/utils.h"

#include <libgen.h>


int streq(char *s1, char *s2) {
    return strcmp(s1, s2) == 0;
}


/* Depending on the implementation, dirname and basename may or may not modify
 * their input string, and may or may not use an internal, unfree-able buffer.
 * That's a lousy contract. We can do better.
 */

char *filepath(char *path) {
    char *temp = strdup(path);
    char *root = strdup(dirname(temp));
    // free(temp);

    return root;
}

char *filename(char *path) {
    char *temp = strdup(path);
    char *name = strdup(basename(temp));
    // free(temp);

    return name;
}
