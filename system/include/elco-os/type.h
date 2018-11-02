#ifndef __ELCO_OS_TYPE_H
#define __ELCO_OS_TYPE_H

#include <kstdint.h>

typedef uint32_t vir_clicks;  /*  virtual addr/length in clicks */
typedef uint32_t phys_clicks; /* physical addr/length in clicks */

typedef uint32_t vir_bytes; /* virtual addr/length in bytes */
#define VIR_BYTES_MAX UINT32_MAX

typedef uint32_t phys_bytes; /* physical addr/length in bytes */
#define PHYS_BYTES_MAX UINT32_MAX

typedef const char pointer_t[]; /* pointer type */

#endif