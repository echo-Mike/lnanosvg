#ifndef LNS_NANOSVG_H__
#define LNS_NANOSVG_H__

/// LNS
#include "lns_includes.h"

/// NANOSVG
#include <nanosvg.h>
#include <nanosvgrast.h>

typedef struct LNS_alloc_t
{
    void *(*malloc_)(size_t);
    void *(*realloc_)(void *, size_t);
    void  (*free_)(void *);
} LNS_alloc;

// Initializes thread local allocator wrapper with allocator from Lua state
//! Must be called before any call to lns_get_allocator on the same thread
void lns_init_allocator(lua_State* L);
// Returns allocator wrapper
LNS_alloc* lns_get_allocator(void);

// Convinient macro wrappers over Lua lns_get_allocator wrapper
#define LNS_MALLOC(size) lns_get_allocator()->malloc_(size)
#define LNS_REALLOC(ptr, size) lns_get_allocator()->realloc_(ptr, size)
#define LNS_FREE(ptr) lns_get_allocator()->free_(ptr)

#endif // LNS_NANOSVG_H__