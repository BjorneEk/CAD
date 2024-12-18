/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * error message printing
 *
 *==========================================================*/

#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdarg.h>

#define ASSERT(_expr, ...) if(!(_expr)) { exit_error_custom("Assertion Error", __VA_ARGS__); }

void verror(const char * fmt, va_list args);

void verror_custom(const char *error, const char * fmt, va_list args);

void error(const char * fmt, ...);

void error_custom(const char *error, const char * fmt, ...);

__attribute__((noreturn))
void exit_error(const char * fmt, ...);

__attribute__((noreturn))
void exit_error_custom(const char *error, const char * fmt, ...);

#endif /* _ERROR_H_ */