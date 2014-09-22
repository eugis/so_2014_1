#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../inc/fixtures.h"


int main(int argc, char* argv[]) {
    char *fixtures = argv[1];
    char *database = argv[2];

    printf("Recreating database at %s (from %s)...\n", database, fixtures);

    fixtures_read(argv[1], argv[2]);

    printf("done.\n");
}