#ifndef gp_h_included_20240607
#define gp_h_included_20240607

typedef int (*gprintf_func_t)(void * , const void *, size_t);
int vgprintf(gprintf_func_t func, void *arg, const char *fmt, va_list *ap);
int gprintf(gprintf_func_t func, void *arg, const char *fmt, ...);

#endif
