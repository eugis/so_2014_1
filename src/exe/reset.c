#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../inc/fixtures.h"


int main(int argc, char* argv[]) {
    char *database = argv[1];
    char *fixtures = argv[2];

    printf("Recreating database at %s (from %s)...\n", database, fixtures);

    fixtures_read(database, fixtures);

    printf("done.\n");
}