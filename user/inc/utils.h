#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

int ram_printf(const char *str, ...);
void __assert_func(const char *file, int line, const char *func, const char *failedexpr);

#endif
