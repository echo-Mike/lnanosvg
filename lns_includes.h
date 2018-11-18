#ifndef LNS_INCLUDES_H__
#define LNS_INCLUDES_H__

#define __STDC_WANT_LIB_EXT1__ 1
/// STD
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#ifndef __STDC_NO_THREADS__
#   include <threads.h>
#   define LNS_THREAD_LOCAL thread_local
#else
#   define LNS_THREAD_LOCAL static
#endif

/// LUA
#include "lua.h"
#include "lauxlib.h"

#endif // LNS_INCLUDES_H__