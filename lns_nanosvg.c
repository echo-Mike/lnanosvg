
#include "lns_nanosvg_imp.h"

typedef struct LNS_L_alloc_t 
{
    void* ud;
    lua_Alloc l_alloc;
} LNS_L_alloc;

LNS_THREAD_LOCAL LNS_L_alloc LNS_allocator_data = {NULL, NULL};

static void* LNS_L_malloc(size_t size)
{
    return LNS_allocator_data.l_alloc(LNS_allocator_data.ud, NULL, 0, size);
}

static void* LNS_L_realloc(void* ptr, size_t size)
{
    return LNS_allocator_data.l_alloc(LNS_allocator_data.ud, ptr, 1, size);
}

static void LNS_L_free(void* ptr)
{
    return LNS_allocator_data.l_alloc(LNS_allocator_data.ud, ptr, 1, 0);
}

static LNS_alloc LNS_allocators = {LNS_L_malloc, LNS_L_realloc, LNS_L_free};

LNS_alloc* lns_get_allocator(void)
{
    return &LNS_allocators;
}

void lns_init_allocator(lua_State* L)
{
    LNS_allocator_data.l_alloc = lua_getallocf(L, &LNS_allocator_data.ud);
}

//$------------------------------------------------------------------------------------------------
// ALLOCATOR DISPLACEMENT FOR NANOSVG
//#------------------------------------------------------------------------------------------------

#if defined(malloc)
#   warning "LUA_NANOSVG: Can't reset malloc : defined as macro : pragma push_macro will be used"
#   pragma push_macro("malloc")
#   define LNS_MALLOC_PUSHED 
#endif
#define malloc lns_get_allocator()->malloc_

#if defined(realloc)
#   warning "LUA_NANOSVG: Can't reset realloc : defined as macro : pragma push_macro will be used"
#   pragma push_macro("realloc")
#   define LNS_REALLOC_PUSHED 
#endif
#define realloc lns_get_allocator()->realloc_

#if defined(free)
#   warning "LUA_NANOSVG: Can't reset free : defined as macro : pragma push_macro will be used"
#   pragma push_macro("free")
#   define LNS_FREE_PUSHED 
#endif
#define free lns_get_allocator()->free_

//$------------------------------------------------------------------------------------------------
// NANOSVG IMPLEMENTATION
//#------------------------------------------------------------------------------------------------

/// NANOSVG
#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

#if defined(LNS_MALLOC_PUSHED)
#   pragma pop_macro("malloc")
#endif

#if defined(LNS_REALLOC_PUSHED)
#   pragma pop_macro("realloc")
#endif

#if defined(LNS_FREE_PUSHED)
#   pragma pop_macro("free")
#endif