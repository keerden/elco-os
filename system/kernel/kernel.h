#ifndef _KERNEL_H
#define _KERNEL_H

#define _ELCO_OS 1
#define _SYSTEM 1

#include <elco-os/config.h>
#include <elco-os/const.h>
#include <elco-os/type.h>

#include <kstddef.h>
#include <kstdint.h>

#if ARCHITECTURE == _ARCH_I386
    #include "i386/i386.h"
#endif

#include "config.h"
#include "const.h"
#include "type.h"
#include "glo.h"
#include "proto.h"



#endif