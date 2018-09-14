#ifndef ERRORS_H_
#define ERRORS_H_

#define MAX_ERROR 255   // The maximum length of an error message

char error_msg[MAX_ERROR];

extern void warn();
extern void die(int exit_code);
extern void error_init_log();
extern void log_error();

#endif
