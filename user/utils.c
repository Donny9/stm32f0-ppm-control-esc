#include <stdio.h>
#include <string.h>
#include <sys.h>
#include <stdarg.h>

extern uint8_t __LogbufBase, __LogbufSize;
static char * const logbuf = (char *)&__LogbufBase;
static const uint32_t logbuf_size = (uint32_t)&__LogbufSize;

static uint32_t log_idx = 0;
int bb_vprintf(const char *str, va_list ap)
{
	static char buf[256];
	int ret, num;
	uint32_t modidx = log_idx % logbuf_size;

	ret = vsnprintf(buf, sizeof(buf), str, ap);
	buf[sizeof(buf) - 1] = '\0';

	num = ret > sizeof(buf) ? sizeof(buf) : ret;

	/* simply overwrite the oldest log when full */
	if (modidx + num <= logbuf_size) {
		memcpy(logbuf + modidx, buf, num);
	} else {
		memcpy(logbuf + modidx, buf, logbuf_size - modidx);
		memcpy(logbuf, buf + logbuf_size - modidx, num + modidx - logbuf_size);
	}
	log_idx += num;

	return num;
}

int ram_printf(const char *str, ...)
{
	va_list ap;
	int ret;

	va_start(ap, str);
	ret = bb_vprintf(str, ap);
	va_end(ap);

	return ret;
}

void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
	uint32_t *stack = __builtin_frame_address(0);
	uint32_t i;

	ram_printf("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
			failedexpr, file, line,
			func ? ", function: " : "", func ? func : "");

	if (stack) {
		ram_printf("stack backtrace:\n");
		for (i = 0; i < 64; i += 4)
			ram_printf ("\t%p: %08x %08x %08x %08x\n", stack + i,
					stack[i], stack[i + 1], stack[i + 2], stack[i + 3]);
	}
	while(1);
}

