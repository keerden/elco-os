#ifndef _KSTDIO_H
#define _KSTDIO_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <kstddef.h>
#include <kstdarg.h>


//typedef FILE
//typedef fpos_t


// #define _IOFBF
// #define _IOLBF
// #define _IONBF

/* IO Buffer Size */
#define BUFSIZ 512

// #define EOF
// #define FOPEN_MAX
// #define FILENAME_MAX




int kprintf(const char* __restrict, ...);
int kputchar(int);
int kputs(const char*);

#ifdef __cplusplus
}
#endif

#endif
