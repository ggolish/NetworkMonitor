#include "errors.h"

#include <stdio.h>
#include <stdlib.h>

static FILE *error_log;

void warn()
{
    fprintf(stderr, "Warning: %s.\n", error_msg);
}

void die(int exit_code)
{
    fprintf(stderr, "Error: %s.\n", error_msg);
    exit(exit_code);
}

void error_init_log()
{
    error_log = fopen("netmon.log", "w");
}

void log_error()
{
    printf("%s\n", error_msg);
    fprintf(error_log, "%s\n", error_msg);
}
