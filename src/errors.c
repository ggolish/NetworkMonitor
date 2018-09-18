#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    fwrite(error_msg, sizeof(char), strlen(error_msg), error_log);
    fwrite("\n", sizeof(char), 1, error_log);
}
