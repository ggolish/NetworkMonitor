#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void warn()
{
    fprintf(stderr, "Warning: %s.\n", error_msg);
}

void die(int exit_code)
{
    fprintf(stderr, "Error: %s.\n", error_msg);
    exit(exit_code);
}
