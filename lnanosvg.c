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

//$------------------------------------------------------------------------------------------------
// ALLOCATOR DISPLACEMENT FOR NANOSVG
//#------------------------------------------------------------------------------------------------

typedef void*(*LNS_malloc_t)(size_t);
typedef void*(*LNS_realloc_t)(void*, size_t);
typedef void(*LNS_free_t)(void*);

typedef struct LNS_alloc_t
{
    LNS_malloc_t malloc_;
    LNS_realloc_t realloc_;
    LNS_free_t free_;
} LNS_alloc;

static LNS_alloc* getLNSAlloc(void);

static LNS_alloc LNS_global_alloc = { malloc, realloc, free };

#if defined(malloc)
#   warning "LUA_NANOSVG: Can't reset malloc : defined as macro : pragma push_macro will be used"
#   pragma push_macro("malloc")
#   define LNS_MALLOC_PUSHED 
#endif
#define malloc getLNSAlloc()->malloc_
#define LNS_MALLOC(size) getLNSAlloc()->malloc_(size)

#if defined(realloc)
#   warning "LUA_NANOSVG: Can't reset realloc : defined as macro : pragma push_macro will be used"
#   pragma push_macro("realloc")
#   define LNS_REALLOC_PUSHED 
#endif
#define realloc getLNSAlloc()->realloc_
#define LNS_REALLOC(ptr, size) getLNSAlloc()->realloc_(ptr, size)

#if defined(free)
#   warning "LUA_NANOSVG: Can't reset free : defined as macro : pragma push_macro will be used"
#   pragma push_macro("free")
#   define LNS_FREE_PUSHED 
#endif
#define free getLNSAlloc()->free_
#define LNS_FREE(ptr) getLNSAlloc()->free_(ptr)

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
#else
#   undef malloc
#endif

#if defined(LNS_REALLOC_PUSHED)
#   pragma pop_macro("realloc")
#else
#   undef realloc
#endif

#if defined(LNS_FREE_PUSHED)
#   pragma pop_macro("free")
#else
#   undef free
#endif

//$------------------------------------------------------------------------------------------------
// MACRO
//#------------------------------------------------------------------------------------------------

/**
#if defined(__alignof_is_defined)
#   define LNS_ALIGNOF(type) alignof(type)
#else
#   define LNS_ALIGNOF(type) offsetof (struct { char c; type member; }, member)
#endif
**/

#if defined(__STDC_LIB_EXT1__)
#   define LNS_STRLEN(str, size) strnlen_s(str, (size))
#else
#   define LNS_STRLEN(str, size) strlen(str)
#endif

#define LNS_DEFER_(x) x

#define LNS_STRINGIFY_(x) #x
#define LNS_STRINGIFY(x) LNS_STRINGIFY_(x)

#define LNS_GET_TYPENAME(name) NSVG_ ## name ## _udata

#define LNS_GET_STRUCT_STR(type) NSVG_struct_ ## type

#define LNS_GEN_REPR_STRUCT_(name)       \
    typedef struct NSVG_##name##_udata_t \
    {                                    \
        NSVG##name copy;                 \
        NSVG##name *ptr;                 \
    } NSVG_##name##_udata

#define LNS_GEN_REPR_STRUCT(name) \
    LNS_GEN_REPR_STRUCT_(name);   \
    static const char NSVG_struct_##name[] = LNS_STRINGIFY(LNS_GEN_REPR_STRUCT_(name))

#define LNS_GEN_ITER_STRUCT_(name)       \
    typedef struct NSVG_##name##_udata_t \
    {                                    \
        NSVG##name *ptr;                 \
    } NSVG_##name##_udata

#define LNS_GEN_ITER_STRUCT(name) \
    LNS_GEN_ITER_STRUCT_(name);   \
    static const char NSVG_struct_##name[] = LNS_STRINGIFY(LNS_GEN_ITER_STRUCT_(name))

#define LNS_LUA_TYPE(type) "lnanosvg." #type

#define check_nsvgimage(L, n)        (LNS_GET_TYPENAME(image)*)luaL_checkudata(L, n, LNS_LUA_TYPE(image))
#define check_nsvgshape(L, n)        (LNS_GET_TYPENAME(shape)*)luaL_checkudata(L, n, LNS_LUA_TYPE(shape))
#define check_nsvgpath(L, n)         (LNS_GET_TYPENAME(path)*)luaL_checkudata(L, n, LNS_LUA_TYPE(path))
#define check_nsvgpaint(L, n)        (LNS_GET_TYPENAME(paint)*)luaL_checkudata(L, n, LNS_LUA_TYPE(paint))
#define check_nsvggradient(L, n)     (LNS_GET_TYPENAME(gradient)*)luaL_checkudata(L, n, LNS_LUA_TYPE(gradient))
#define check_nsvggradientStop(L, n) (LNS_GET_TYPENAME(gradientStop)*)luaL_checkudata(L, n, LNS_LUA_TYPE(gradientStop))

#define LNS_LALLOC_UDATA(L, varname, type) \
    LNS_GET_TYPENAME(type) *varname = (LNS_GET_TYPENAME(type) *)lua_newuserdata(L, sizeof(LNS_GET_TYPENAME(type)))

#define LNS_CHECK_AND_GET(L, varname, pos, type) \
    LNS_GET_TYPENAME(type) *varname = LNS_DEFER_(LNS_DEFER_(check_nsvg##type)(L, pos))

#define LNS_MEMBER_FUNC(type, funcname) lnanosvg_ ## type ## _ ## funcname

#define LNS_LUA_MEMBER_FUNCTION(type, funcname) static int LNS_MEMBER_FUNC(type, funcname)(lua_State* L)

#define LNS_INIT_ALLOCATOR(L) LNS_allocator_data.l_alloc = lua_getallocf(L, &LNS_allocator_data.ud)

#define LNS_CONSTRUCTOR(type, nsvgtype)                                                                  \
    LNS_LUA_MEMBER_FUNCTION(type, new)                                                                   \
    {                                                                                                    \
        if (!lua_islightuserdata(L, 1))                                                                  \
            return luaL_error(L, "Provided type is not light user data, type: %s", luaL_typename(L, 1)); \
        nsvgtype *p = (nsvgtype *)lua_touserdata(L, 1);                                                  \
        lua_pop(L, 1);                                                                                   \
        LNS_LALLOC_UDATA(L, data, type);                                                                 \
        luaL_getmetatable(L, LNS_LUA_TYPE(type));                                                        \
        lua_setmetatable(L, -2);                                                                         \
        data->copy = *p;                                                                                 \
        data->ptr = p;                                                                                   \
        return 1;                                                                                        \
    }

#define LNS_CONSTRUCTOR_ITER(type, nsvgtype)                                                             \
    LNS_LUA_MEMBER_FUNCTION(type, new)                                                                   \
    {                                                                                                    \
        if (!lua_islightuserdata(L, 1))                                                                  \
            return luaL_error(L, "Provided type is not light user data, type: %s", luaL_typename(L, 1)); \
        nsvgtype *p = (nsvgtype *)lua_touserdata(L, 1);                                                  \
        lua_pop(L, 1);                                                                                   \
        LNS_LALLOC_UDATA(L, data, type);                                                                 \
        luaL_getmetatable(L, LNS_LUA_TYPE(type));                                                        \
        lua_setmetatable(L, -2);                                                                         \
        data->ptr = p;                                                                                   \
        return 1;                                                                                        \
    }

//$------------------------------------------------------------------------------------------------
// STRUCTS AND STATICS
//#------------------------------------------------------------------------------------------------

LNS_GEN_REPR_STRUCT(image);
LNS_GEN_ITER_STRUCT(shape);
LNS_GEN_ITER_STRUCT(path);
LNS_GEN_REPR_STRUCT(paint);
LNS_GEN_REPR_STRUCT(gradient);
LNS_GEN_REPR_STRUCT(gradientStop);

typedef struct LNS_DotIterator_t
{
    int npts, current;
    float* begin;
} LNS_DotIterator;

typedef struct LNS_GradStopIterator_t
{
    int nstops, current;
    NSVGgradientStop* begin;
} LNS_GradStopIterator;

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

static LNS_alloc* getLNSAlloc(void)
{
    return &LNS_allocators;
}

//$------------------------------------------------------------------------------------------------
// FREE FUNCTIONS
//#------------------------------------------------------------------------------------------------

static int lnanosvg_units_check(const char* units)
{
	if (units[0] == 'p' && units[1] == 'x')
		return 1;
	else if (units[0] == 'p' && units[1] == 't')
		return 1;
	else if (units[0] == 'p' && units[1] == 'c')
		return 1;
	else if (units[0] == 'm' && units[1] == 'm')
		return 1;
	else if (units[0] == 'c' && units[1] == 'm')
		return 1;
	else if (units[0] == 'i' && units[1] == 'n')
		return 1;
	else if (units[0] == '%')
		return 1;
	else if (units[0] == 'e' && units[1] == 'm')
		return 1;
	else if (units[0] == 'e' && units[1] == 'x')
		return 1;
	return 0;
}

//$------------------------------------------------------------------------------------------------
// OBJECT ALLOCATION
//#------------------------------------------------------------------------------------------------

//% IMAGE

LNS_LUA_MEMBER_FUNCTION(image, fromFile)
{
    NSVGimage* img = NULL;
    const char* filename = luaL_checkstring(L, 1);
    const char* units = luaL_checkstring(L, 2);
    float dpi = luaL_checknumber(L, 3);
    
    luaL_argcheck(L, filename != NULL, 1, "file path must be provided");
    luaL_argcheck(L, units != NULL, 2, "units type must be provided");
    luaL_argcheck(L, lnanosvg_units_check(units), 2, "units must have format \"px\"/\"pt\"/\"pc\"/\"mm\"/\"cm\"/\"in\"/\"em\"/\"ex\" or \"%percent_value\"");
    
    LNS_INIT_ALLOCATOR(L);
    img = nsvgParseFromFile(filename, units, dpi);
    if (img == NULL)
        return luaL_error(L, "Can't create svg image object from file: %s", filename);
    
    LNS_LALLOC_UDATA(L, data, image);
    luaL_getmetatable(L, LNS_LUA_TYPE(image));
    lua_setmetatable(L, -2);

    data->copy = *img;
    data->ptr = img;
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(image, fromString)
{
    NSVGimage* img = NULL;
    const char* string = luaL_checkstring(L, 1);
    const char* units = luaL_checkstring(L, 2);
    float dpi = luaL_checknumber(L, 3);
    size_t size = LNS_STRLEN(string, (((size_t)1) << 22));
    LNS_INIT_ALLOCATOR(L);

    luaL_argcheck(L, string != NULL, 1, "source string must be provided");
    luaL_argcheck(L, units != NULL, 2, "units type must be provided");
    luaL_argcheck(L, lnanosvg_units_check(units), 2, "units must have format \"px\"/\"pt\"/\"pc\"/\"mm\"/\"cm\"/\"in\"/\"em\"/\"ex\" or \"%percent_value\"");

    char* src = (char*)LNS_MALLOC(size);
    if (src == NULL)
        return luaL_error(L, "Can't allocate temporary storage for SVG image source. Requested size: %d", size);
    memcpy(src, string, size);

    img = nsvgParse(src, units, dpi);
    if (img == NULL)
    {
        LNS_FREE(src);
        return luaL_error(L, "Can't create svg image object");
    }
    
    LNS_LALLOC_UDATA(L, data, image);
    luaL_getmetatable(L, LNS_LUA_TYPE(image));
    lua_setmetatable(L, -2);

    LNS_FREE(src);

    data->copy = *img;
    data->ptr = img;

    return 1;
}

//% SHAPE

LNS_CONSTRUCTOR_ITER(shape, NSVGshape)

//% PATH

LNS_CONSTRUCTOR_ITER(path, NSVGpath)

//% PAINT

LNS_CONSTRUCTOR(paint, NSVGpaint)

//% GRADIENT

LNS_CONSTRUCTOR(gradient, NSVGgradient)

//% GRADIENT_STOP

LNS_CONSTRUCTOR(gradientStop, NSVGgradientStop)

//$------------------------------------------------------------------------------------------------
// OBJECTS MEMBER FUNCTIONS
//#------------------------------------------------------------------------------------------------

//% IMAGE

static int lnanosvg_iterate_shapes(lua_State* L)
{
    NSVGshape* p = (NSVGshape*)lua_touserdata(L, 1);
    LNS_CHECK_AND_GET(L, s, 2, shape);
    s->ptr = s->ptr == NULL ? p : s->ptr->next;
    if (s->ptr == NULL)
        lua_pushnil(L);
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(image, iterate)
{
    LNS_CHECK_AND_GET(L, img, 1, image);
    lua_pushcfunction(L, lnanosvg_iterate_shapes);
    lua_pushlightuserdata(L, img->copy.shapes);
    lua_pushlightuserdata(L, NULL);
    LNS_MEMBER_FUNC(shape, new)(L);
    return 3;
}

LNS_LUA_MEMBER_FUNCTION(image, getCDecl)
{
    lua_pushstring(L, LNS_GET_STRUCT_STR(image));
    return 1;
}

//% SHAPE

LNS_LUA_MEMBER_FUNCTION(shape, next)
{
    LNS_CHECK_AND_GET(L, s, 1, shape);
    if (s->ptr->next == NULL)
        lua_pushnil(L);
    else
        s->ptr = s->ptr->next;
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(shape, getNext)
{
    LNS_CHECK_AND_GET(L, s, 1, shape);
    if (s->ptr->next != NULL)
    {
        lua_pushlightuserdata(L, s->ptr->next);
        LNS_MEMBER_FUNC(shape, new)(L);
    }
    else
        lua_pushnil(L);
    return 1;
}

static int lnanosvg_iterate_paths(lua_State* L)
{
    NSVGpath* init_ptr = (NSVGpath*)lua_touserdata(L, 1);
    LNS_CHECK_AND_GET(L, p, 2, path);
    p->ptr = p->ptr == NULL ? init_ptr : p->ptr->next;
    if (p->ptr == NULL)
        lua_pushnil(L);
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(shape, iterate)
{
    LNS_CHECK_AND_GET(L, s, 1, shape);
    lua_pushcfunction(L, lnanosvg_iterate_paths);
    lua_pushlightuserdata(L, s->ptr->paths);
    lua_pushlightuserdata(L, NULL);
    LNS_MEMBER_FUNC(shape, new)(L);
    return 3;
}

LNS_LUA_MEMBER_FUNCTION(shape, getCDecl)
{
    lua_pushstring(L, LNS_GET_STRUCT_STR(shape));
    return 1;
}

//% PATH

LNS_LUA_MEMBER_FUNCTION(path, next)
{
    LNS_CHECK_AND_GET(L, path, 1, path);
    if (path->ptr->next == NULL)
        lua_pushnil(L);
    else
        path->ptr = path->ptr->next;
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(path, getNext)
{
    LNS_CHECK_AND_GET(L, path, 1, path);
    if (path->ptr->next != NULL)
    {
        lua_pushlightuserdata(L, path->ptr->next);
        LNS_MEMBER_FUNC(path, new)(L);
    }
    else
        lua_pushnil(L);
    return 1;
}

static int lnanosvg_iterate_dots(lua_State* L)
{
    LNS_DotIterator* iter = (LNS_DotIterator*)lua_touserdata(L, 1);
    if (iter->current < iter->npts-1)
    {
        float* p = &iter->begin[iter->current*2];
        lua_createtable(L, 8, 0);
        for (int i = 0; i < 8; ++i)
        {
            lua_pushinteger(L, i+1);
            lua_pushnumber(L, p[i]);
            lua_settable(L, -3);
        }
        iter->current += 3;
    }
    else
        lua_pushnil(L);

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(path, iterate)
{
    LNS_CHECK_AND_GET(L, path, 1, path);
    lua_pushcfunction(L, lnanosvg_iterate_dots);
    LNS_DotIterator* iter = (LNS_DotIterator*)lua_newuserdata(L, sizeof(LNS_DotIterator));
    iter->npts = path->ptr->npts;
    iter->current = 0;
    iter->begin = path->ptr->pts;
    lua_pushnil(L);
    return 3;
}

LNS_LUA_MEMBER_FUNCTION(path, getCDecl)
{
    lua_pushstring(L, LNS_GET_STRUCT_STR(path));
    return 1;
}

//% PAINT

LNS_LUA_MEMBER_FUNCTION(paint, toColor)
{
    LNS_CHECK_AND_GET(L, paint, 1, paint);
    lua_createtable(L, 4, 0);
    if (paint->copy.type == NSVG_PAINT_NONE            ||
        paint->copy.type == NSVG_PAINT_LINEAR_GRADIENT ||
        paint->copy.type == NSVG_PAINT_RADIAL_GRADIENT)
    {
        lua_pushnil(L);
        return 1;
    }
    for (int i = 0; i < 3; ++i)
    { // r,g,b
        lua_pushinteger(L, i+1);
        lua_pushinteger(L, (paint->copy.color >> (i * 8)) & 0xFF);
        lua_settable(L, -3);
    }
    // alfa
    lua_pushinteger(L, 4);
    lua_pushinteger(L, 255);
    lua_settable(L, -3);

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(paint, toGradient)
{
    LNS_CHECK_AND_GET(L, paint, 1, paint);
    if (paint->copy.type == NSVG_PAINT_NONE ||
        paint->copy.type == NSVG_PAINT_COLOR)
    {
        lua_pushnil(L);
        return 1;
    }
    lua_pushlightuserdata(L, paint->copy.gradient);
    LNS_MEMBER_FUNC(gradient, new)(L);
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(paint, getCDecl)
{
    lua_pushstring(L, LNS_GET_STRUCT_STR(paint));
    return 1;
}

//% GRADIENT

static int lnanosvg_iterate_gradstop(lua_State* L)
{
    LNS_GradStopIterator* iter = (LNS_GradStopIterator*)lua_touserdata(L, 1);
    LNS_CHECK_AND_GET(L, gs, 2, gradientStop);
    if (iter->current < iter->nstops)
    {
        gs->copy = iter->begin[iter->current];
        gs->ptr = iter->begin + iter->current;
        ++iter->current;
    }
    else
        lua_pushnil(L);

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(gradient, iterate)
{
    LNS_CHECK_AND_GET(L, grad, 1, gradient);
    lua_pushcfunction(L, lnanosvg_iterate_dots);
    LNS_GradStopIterator* iter = (LNS_GradStopIterator*)lua_newuserdata(L, sizeof(LNS_GradStopIterator));
    iter->nstops = grad->copy.nstops;
    iter->current = 0;
    iter->begin = grad->ptr->stops;
    lua_pushlightuserdata(L, grad->copy.stops); //? grad->copy.stops - is a trash data but not NULL
    LNS_MEMBER_FUNC(gradientStop, new)(L);
    return 3;
}

LNS_LUA_MEMBER_FUNCTION(gradient, getCDecl)
{
    lua_pushstring(L, LNS_GET_STRUCT_STR(gradient));
    return 1;
}

//% GRADIENT_STOP

LNS_LUA_MEMBER_FUNCTION(gradientStop, toColor)
{
    LNS_CHECK_AND_GET(L, grs, 1, gradientStop);
    lua_createtable(L, 4, 0);
    for (int i = 0; i < 3; ++i)
    { // r,g,b
        lua_pushinteger(L, i+1);
        lua_pushinteger(L, (grs->copy.color >> (i * 8)) & 0xFF);
        lua_settable(L, -3);
    }
    // alfa
    lua_pushinteger(L, 4);
    lua_pushinteger(L, 255);
    lua_settable(L, -3);

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(gradientStop, getCDecl)
{
    lua_pushstring(L, LNS_GET_STRUCT_STR(gradientStop));
    return 1;
}

//$------------------------------------------------------------------------------------------------
// OBJECTS META-FUNCTIONS
//#------------------------------------------------------------------------------------------------

//% IMAGE

LNS_LUA_MEMBER_FUNCTION(image, _tostring)
{
    LNS_CHECK_AND_GET(L, img, 1, image);
    size_t size = 0;
    for (NSVGshape *shape = img->copy.shapes; shape != NULL; ++size, shape = shape->next);
    lua_pushfstring(L, "<lnanosvg.image w:%f h:%f shapes:%u> (%p:%p)",
                    img->copy.width, img->copy.height, size, img, img->ptr);
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(image, _index)
{
    LNS_CHECK_AND_GET(L, img, 1, image);
    const char* field = luaL_checkstring(L, 2);
    if (strcmp(field, "width") == 0)
        lua_pushnumber(L, img->copy.width);
    else if (strcmp(field, "height") == 0)
        lua_pushnumber(L, img->copy.height);
    else
    {
        lua_getmetatable(L, 1);
        lua_pushvalue(L, 2);
        lua_gettable(L, -2);
    }

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(image, _newindex)
{
    LNS_CHECK_AND_GET(L, img, 1, image);
    const char* field = luaL_checkstring(L, 2);
    if (strcmp(field, "width") == 0)
        img->copy.width = img->ptr->width = luaL_checknumber(L, 3);
    else if (strcmp(field, "height") == 0)
        img->copy.height = img->ptr->height = luaL_checknumber(L, 3);
    return 0;
}

LNS_LUA_MEMBER_FUNCTION(image, _gc)
{
    LNS_CHECK_AND_GET(L, data, 1, image);
    LNS_INIT_ALLOCATOR(L);
    nsvgDelete(data->ptr);
    memset(data, 0, sizeof(LNS_GET_TYPENAME(image)));
    return 0;
}

//% SHAPE

LNS_LUA_MEMBER_FUNCTION(shape, _tostring)
{
    LNS_CHECK_AND_GET(L, s, 1, shape);
    size_t paths_count = 0;
    const char *lineJoin = NULL, *lineCap = NULL, *fillType = NULL, *strokeType = NULL;
    luaL_Buffer buffer;
    // init
    for (NSVGpath* path_ = s->ptr->paths; path_ != NULL; ++paths_count, path_ = path_->next);
    luaL_buffinit(L, &buffer);
    switch (s->ptr->strokeLineJoin)
    {
        case NSVG_JOIN_MITER: lineJoin = "miter"; break;
        case NSVG_JOIN_ROUND: lineJoin = "round"; break;
        case NSVG_JOIN_BEVEL: lineJoin = "bevel"; break;
    }
    switch (s->ptr->strokeLineCap)
    {
        case NSVG_CAP_BUTT:   lineCap = "butt";   break;
        case NSVG_CAP_ROUND:  lineCap = "round";  break;
        case NSVG_CAP_SQUARE: lineCap = "square"; break;
    }
    switch (s->ptr->fill.type)
    {
        case NSVG_PAINT_NONE:            fillType = "none";              break;
        case NSVG_PAINT_COLOR:           fillType = "color";             break;
        case NSVG_PAINT_LINEAR_GRADIENT: fillType = "linear_gradient";   break;
        case NSVG_PAINT_RADIAL_GRADIENT: fillType = "radial_gradient";   break;
    }
    switch (s->ptr->stroke.type)
    {
        case NSVG_PAINT_NONE:            strokeType = "none";              break;
        case NSVG_PAINT_COLOR:           strokeType = "color";             break;
        case NSVG_PAINT_LINEAR_GRADIENT: strokeType = "linear_gradient";   break;
        case NSVG_PAINT_RADIAL_GRADIENT: strokeType = "radial_gradient";   break;
    }
    // write
    luaL_addstring(&buffer, "<lnanosvg.shape ");

    size_t written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, 
        "opacity:%f miterLimit:%f fillRule:%s visible:%s stroke: { width:%f dashOffset:%f lineJoin:%s lineCap:%s dashArray: [ ",
        s->ptr->opacity, s->ptr->miterLimit, 
        s->ptr->fillRule == NSVG_FILLRULE_NONZERO ? "nonzero" : "evenodd",
        s->ptr->flags & NSVG_FLAGS_VISIBLE ? "true" : "false",
        s->ptr->strokeWidth, s->ptr->strokeDashOffset, lineJoin, lineCap
    );
    luaL_addsize(&buffer, written);

    if (s->ptr->strokeDashCount)
    {
        for (int i = 0; i < s->ptr->strokeDashCount-1; ++i){
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "%f, ", s->ptr->strokeDashArray[i]);
            luaL_addsize(&buffer, written);
        }
        written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "%f ] } ", s->ptr->strokeDashArray[s->ptr->strokeDashCount-1]);
        luaL_addsize(&buffer, written);
    }
    else
    {
        written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "] } ");
        luaL_addsize(&buffer, written);
    }

    written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE,
        "fill_color: [ type:%s ", fillType
    );
    luaL_addsize(&buffer, written);
    switch (s->ptr->fill.type)
    {
        case NSVG_PAINT_NONE:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "] ");
        } break;
        case NSVG_PAINT_COLOR:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "r:%u g:%u b:%u ] ",
                s->ptr->fill.color & 0xFF, (s->ptr->fill.color >> 8) & 0xFF, (s->ptr->fill.color >> 16) & 0xFF);
        } break;
        case NSVG_PAINT_LINEAR_GRADIENT:
            /* fallthrough */
        case NSVG_PAINT_RADIAL_GRADIENT:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, " (%p) ] ", s->ptr->fill.gradient);
        } break;
    }
    luaL_addsize(&buffer, written);

    written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE,
        "stroke_color: [ type:%s ", strokeType
    );
    luaL_addsize(&buffer, written);
    switch (s->ptr->stroke.type)
    {
        case NSVG_PAINT_NONE:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "] ");
        } break;
        case NSVG_PAINT_COLOR:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "r:%u g:%u b:%u ] ",
                s->ptr->stroke.color & 0xFF, (s->ptr->stroke.color >> 8) & 0xFF, (s->ptr->stroke.color >> 16) & 0xFF);
        } break;
        case NSVG_PAINT_LINEAR_GRADIENT:
            /* fallthrough */
        case NSVG_PAINT_RADIAL_GRADIENT:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, " (%p) ] ", s->ptr->stroke.gradient);
        } break;
    }
    luaL_addsize(&buffer, written);

    written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, 
        "bounding_box: [ minx:%f miny:%f maxx:%f maxy:%f ] paths:%u> (%p:%p)",
        s->ptr->bounds[0], s->ptr->bounds[1], s->ptr->bounds[2], s->ptr->bounds[3],
        paths_count, s, s->ptr
    );
    luaL_addsize(&buffer, written);
    // final
    luaL_pushresult(&buffer);
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(shape, _index)
{
    LNS_CHECK_AND_GET(L, s, 1, shape);
    const char* field = luaL_checkstring(L, 2);

    if (strcmp(field, "opacity") == 0)
        lua_pushnumber(L, s->ptr->opacity);
    else if (strcmp(field, "miterLimit") == 0)
        lua_pushnumber(L, s->ptr->miterLimit);
    else if (strcmp(field, "fillRule") == 0)
        lua_pushstring(L, s->ptr->fillRule == NSVG_FILLRULE_NONZERO ? "nonzero" : "evenodd");
    else if (strcmp(field, "visible") == 0)
        lua_pushboolean(L, s->ptr->flags & NSVG_FLAGS_VISIBLE);
    else if (strcmp(field, "bbox") == 0)
    {
        lua_createtable(L, 4, 0);
        for (int i = 0; i < 4; ++i)
        {
            lua_pushinteger(L, i+1);
            lua_pushnumber(L, s->ptr->bounds[i]);
            lua_settable(L, -3);
        }
    }
    else if (strncmp(field, "color.", sizeof("color.") - 1) == 0)
    {
        if (strcmp(field + sizeof("color.") - 1, "fill") == 0)
            lua_pushlightuserdata(L, &s->ptr->fill);
        else if (strcmp(field + sizeof("color.") - 1, "stroke") == 0)
            lua_pushlightuserdata(L, &s->ptr->stroke);
        else
            goto shape_index_meta;
        LNS_MEMBER_FUNC(paint, new)(L);
    }
    else if (strncmp(field, "stroke.", sizeof("stroke.") - 1) == 0)
    {
        const char* subField = field + sizeof("stroke.") - 1;
        if (strcmp(subField, "width") == 0)
            lua_pushnumber(L, s->ptr->strokeWidth);
        else if (strcmp(subField, "dashOffset") == 0)
            lua_pushnumber(L, s->ptr->strokeDashOffset);
        else if (strcmp(subField, "lineJoin") == 0)
            switch (s->ptr->strokeLineJoin)
            {
                case NSVG_JOIN_MITER: lua_pushstring(L, "miter"); break;
                case NSVG_JOIN_ROUND: lua_pushstring(L, "round"); break;
                case NSVG_JOIN_BEVEL: lua_pushstring(L, "bevel"); break;
            }
        else if (strcmp(subField, "lineCap") == 0)
            switch (s->ptr->strokeLineCap)
            {
                case NSVG_CAP_BUTT:   lua_pushstring(L, "butt");   break;
                case NSVG_CAP_ROUND:  lua_pushstring(L, "round");  break;
                case NSVG_CAP_SQUARE: lua_pushstring(L, "square"); break;
            }
        else if (strcmp(subField, "dashArray") == 0)
        {
            lua_createtable(L, s->ptr->strokeDashCount, 0);
            for (int i = 0; i < s->ptr->strokeDashCount; ++i)
            {
                lua_pushinteger(L, i+1);
                lua_pushnumber(L, s->ptr->strokeDashArray[i]);
                lua_settable(L, -3);
            }
        }
        else
            goto shape_index_meta;
    }
    else
    {
        shape_index_meta:
            lua_getmetatable(L, 1);
            lua_pushvalue(L, 2);
            lua_gettable(L, -2);
    }

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(shape, _newindex)
{
    LNS_CHECK_AND_GET(L, s, 1, shape);
    const char* field = luaL_checkstring(L, 2);
    
    if (strcmp(field, "opacity") == 0)
        s->ptr->opacity = luaL_checknumber(L, 3);
    else if (strcmp(field, "miterLimit") == 0)
        s->ptr->miterLimit = luaL_checknumber(L, 3);
    else if (strcmp(field, "fillRule") == 0)
    {
        const char* fillRule = luaL_checkstring(L, 3);
        if (strncmp(fillRule, "nonzero", sizeof("nonzero")-1) == 0)
            s->ptr->fillRule = NSVG_FILLRULE_NONZERO;
        else if (strncmp(fillRule, "evenodd", sizeof("evenodd")-1) == 0)
            s->ptr->fillRule = NSVG_FILLRULE_EVENODD;
    }
    else if (strcmp(field, "visible") == 0)
        s->ptr->flags = lua_gettop(L) == 3 ? s->ptr->flags & NSVG_FLAGS_VISIBLE : s->ptr->flags & ~NSVG_FLAGS_VISIBLE;
    else if (strcmp(field, "bbox") == 0)
    {
        luaL_checktype(L, 3, LUA_TTABLE);
        for (int i = 0; i < 4; ++i)
        {
            lua_pushinteger(L, i+1);
            lua_gettable(L, -2);
            s->ptr->bounds[i] = luaL_checknumber(L , -1);
        }
    }
    else if (strncmp(field, "color.", sizeof("color.") - 1) == 0)
    {
        const char* subField = field + sizeof("color.") - 1;
        LNS_CHECK_AND_GET(L, paint, 3, paint);
        if (strcmp(subField, "fill") == 0)
        {
            if (s->ptr->fill.type == NSVG_PAINT_LINEAR_GRADIENT ||
                s->ptr->fill.type == NSVG_PAINT_RADIAL_GRADIENT)
                return luaL_error(L, "can't reset lnanosvg.paint with gradient type");
            s->ptr->fill = paint->copy;
        }
        else if (strcmp(subField, "stroke") == 0)
        {
            if (s->ptr->stroke.type == NSVG_PAINT_LINEAR_GRADIENT ||
                s->ptr->stroke.type == NSVG_PAINT_RADIAL_GRADIENT)
                return luaL_error(L, "can't reset lnanosvg.paint with gradient type");
            s->ptr->stroke = paint->copy;
        }
        else
            return 0;
    }
    else if (strncmp(field, "stroke.", sizeof("stroke.") - 1) == 0)
    {
        const char* subField = field + sizeof("stroke.") - 1;
        if (strcmp(subField, "width") == 0)
            s->ptr->strokeWidth = luaL_checknumber(L, 3);
        else if (strcmp(subField, "dashOffset") == 0)
            s->ptr->strokeDashOffset = luaL_checknumber(L, 3);
        else if (strcmp(subField, "lineJoin") == 0)
        {
            const char* lineJoin = luaL_checkstring(L, 3);
            if (strcmp(lineJoin, "miter") == 0)
                s->ptr->strokeLineJoin = NSVG_JOIN_MITER;
            else if (strcmp(lineJoin, "round") == 0)
                s->ptr->strokeLineJoin = NSVG_JOIN_ROUND;
            else if (strcmp(lineJoin, "bevel") == 0)
                s->ptr->strokeLineJoin = NSVG_JOIN_BEVEL;
        }
        else if (strcmp(subField, "lineCap") == 0)
        {
            const char* lineCap = luaL_checkstring(L, 3);
            if (strcmp(lineCap, "butt") == 0)
                s->ptr->strokeLineJoin = NSVG_CAP_BUTT;
            else if (strcmp(lineCap, "round") == 0)
                s->ptr->strokeLineJoin = NSVG_CAP_ROUND;
            else if (strcmp(lineCap, "square") == 0)
                s->ptr->strokeLineJoin = NSVG_CAP_SQUARE;
        }
        else if (strcmp(subField, "dashArray") == 0)
        {
            luaL_checktype(L, 3, LUA_TTABLE);
            char i = 0;
            for (; i < 8; ++i)
            {
                lua_pushinteger(L, i+1);
                lua_gettable(L, -2);
                if (lua_isnil(L, -1))
                    break;
                s->ptr->strokeDashArray[i] = luaL_checknumber(L , -1);
            }
            s->ptr->strokeDashCount = i;
        }
    }
    return 0;
}

//% PATH

LNS_LUA_MEMBER_FUNCTION(path, _tostring)
{
    LNS_CHECK_AND_GET(L, path, 1, path);
    lua_pushfstring(L, 
        "<lnanosvg.path points_count:%d closed:%s bounding_box: [minx:%f miny:%f maxx:%f maxy:%f]> (%p:%p)",
        path->ptr->npts, path->ptr->closed ? "true" : "false", 
        path->ptr->bounds[0], path->ptr->bounds[1], path->ptr->bounds[2], path->ptr->bounds[3],
        path, path->ptr
    );
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(path, _index)
{
    LNS_CHECK_AND_GET(L, path, 1, path);
    const char* field = luaL_checkstring(L, 2);

    if (strcmp(field, "points_count") == 0)
        lua_pushinteger(L, path->ptr->npts);
    else if (strcmp(field, "closed") == 0)
        lua_pushboolean(L, path->ptr->closed);
    else if (strcmp(field, "bbox") == 0)
    {
        lua_createtable(L, 4, 0);
        for (int i = 0; i < 4; ++i)
        {
            lua_pushinteger(L, i+1);
            lua_pushnumber(L, path->ptr->bounds[i]);
            lua_settable(L, -3);
        }
    }
    else
    {
        lua_getmetatable(L, 1);
        lua_pushvalue(L, 2);
        lua_gettable(L, -2);
    }

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(path, _newindex)
{
    LNS_CHECK_AND_GET(L, path, 1, path);
    const char* field = luaL_checkstring(L, 2);

    if (strcmp(field, "closed") == 0)
        path->ptr->closed = lua_gettop(L) == 3;
    else if (strcmp(field, "bbox") == 0)
    {
        luaL_checktype(L, 3, LUA_TTABLE);
        for (int i = 0; i < 4; ++i)
        {
            lua_pushinteger(L, i+1);
            lua_gettable(L, -2);
            path->ptr->bounds[i] = luaL_checknumber(L , -1);
        }
    }

    return 0;
}

//% PAINT

LNS_LUA_MEMBER_FUNCTION(paint, _tostring)
{
    LNS_CHECK_AND_GET(L, paint, 1, paint);
    const char *type = NULL;
    luaL_Buffer buffer;

    switch (paint->copy.type)
    {
        case NSVG_PAINT_NONE:            type = "none";            break;
        case NSVG_PAINT_COLOR:           type = "color";           break;
        case NSVG_PAINT_LINEAR_GRADIENT: type = "linear_gradient"; break;
        case NSVG_PAINT_RADIAL_GRADIENT: type = "radial_gradient"; break;
    }
    luaL_buffinit(L, &buffer);
    luaL_addstring(&buffer, "<lnanosvg.paint ");

    size_t written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "<lnanosvg.paint type:%s [ ", type);
    luaL_addsize(&buffer, written);

    switch (paint->copy.type)
    {
        case NSVG_PAINT_NONE:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "]> (%p:%p)", paint, paint->ptr);
        } break;
        case NSVG_PAINT_COLOR:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, "r:%u g:%u b:%u ]> (%p:%p)",
                paint->copy.color & 0xFF, (paint->copy.color >> 8) & 0xFF, (paint->copy.color >> 16) & 0xFF);
        } break;
        case NSVG_PAINT_LINEAR_GRADIENT:
            /* fallthrough */
        case NSVG_PAINT_RADIAL_GRADIENT:
        {
            written = snprintf(luaL_prepbuffer(&buffer), LUAL_BUFFERSIZE, " (%p) ]> (%p:%p)", paint->copy.gradient);
        } break;
    }
    luaL_addsize(&buffer, written);

    luaL_pushresult(&buffer);
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(paint, _index)
{
    LNS_CHECK_AND_GET(L, paint, 1, paint);
    const char* field = luaL_checkstring(L, 2);
    const char* type = NULL;

    switch (paint->copy.type)
    {
        case NSVG_PAINT_NONE:            type = "none";            break;
        case NSVG_PAINT_COLOR:           type = "color";           break;
        case NSVG_PAINT_LINEAR_GRADIENT: type = "linear_gradient"; break;
        case NSVG_PAINT_RADIAL_GRADIENT: type = "radial_gradient"; break;
    }

    if (strcmp(field, "type") == 0)
        lua_pushstring(L, type);
    else
    {
        lua_getmetatable(L, 1);
        lua_pushvalue(L, 2);
        lua_gettable(L, -2);
    }

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(paint, _newindex)
{
    LNS_CHECK_AND_GET(L, paint, 1, paint);
    const char* field = luaL_checkstring(L, 2);
    unsigned int new_color = 0;

    if (strcmp(field, "color") == 0)
    {
        if (paint->copy.type == NSVG_PAINT_LINEAR_GRADIENT ||
            paint->copy.type == NSVG_PAINT_RADIAL_GRADIENT)
            return 0;
        if (lua_istable(L, 3))
        {
            for (int i = 0; i < 3; ++i)
            {
                lua_pushinteger(L, i+1);
                lua_gettable(L, -2);
                new_color |= luaL_checkinteger(L, -1) << (i * 8);
            }
            paint->ptr->color = paint->copy.color = new_color;
        }
        else if (lua_isnoneornil(L, 3))
        {
            paint->ptr->type = paint->copy.type = NSVG_PAINT_NONE;
            paint->ptr->color = paint->copy.color = new_color;
        }
    }

    return 0;
}

//% GRADIENT

LNS_LUA_MEMBER_FUNCTION(gradient, _tostring)
{
    LNS_CHECK_AND_GET(L, grad, 1, gradient);
    lua_pushfstring(L, 
        "<lnanosvg.gradient spread:%s fx:%f fy:%f stops_count:%d xform: [ %f %f %f %f %f %f ]> (%p:%p)",
        grad->copy.spread ? "true" : "false", 
        grad->copy.fx, grad->copy.fy, grad->copy.nstops,
        grad->copy.xform[0], grad->copy.xform[1], grad->copy.xform[2], grad->copy.xform[3], grad->copy.xform[4], grad->copy.xform[5], 
        grad, grad->ptr
    );
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(gradient, _index)
{
    LNS_CHECK_AND_GET(L, grad, 1, gradient);
    const char* field = luaL_checkstring(L, 2);

    if (strcmp(field, "stops_count") == 0)
        lua_pushinteger(L, grad->copy.nstops);
    else if (strcmp(field, "spread") == 0)
        lua_pushboolean(L, grad->copy.spread);
    else if (strcmp(field, "fx") == 0)
        lua_pushnumber(L, grad->copy.fx);
    else if (strcmp(field, "fy") == 0)
        lua_pushnumber(L, grad->copy.fy);
    else if (strcmp(field, "xform") == 0)
    {
        lua_createtable(L, 6, 0);
        for (int i = 0; i < 6; ++i)
        {
            lua_pushinteger(L, i+1);
            lua_pushnumber(L, grad->copy.xform[i]);
            lua_settable(L, -3);
        }
    }
    else
    {
        lua_getmetatable(L, 1);
        lua_pushvalue(L, 2);
        lua_gettable(L, -2);
    }

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(gradient, _newindex)
{
    LNS_CHECK_AND_GET(L, grad, 1, gradient);
    const char* field = luaL_checkstring(L, 2);

    /*if (strcmp(field, "stops_count") == 0)
        grad->ptr->nstops = grad->copy.nstops = luaL_checkinteger(L, 3);
    else */
    if (strcmp(field, "spread") == 0)
        grad->ptr->spread = grad->copy.spread = lua_gettop(L) == 3;
    else if (strcmp(field, "fx") == 0)
        grad->ptr->fx = grad->copy.fx = luaL_checknumber(L, 3);
    else if (strcmp(field, "fy") == 0)
        grad->ptr->fy = grad->copy.fy = luaL_checknumber(L, 3);
    else if (strcmp(field, "xform") == 0)
    {
        luaL_checktype(L, 3, LUA_TTABLE);
        for (int i = 0; i < 6; ++i)
        {
            lua_pushinteger(L, i+1);
            lua_gettable(L, -2);
            grad->ptr->xform[i] = grad->copy.xform[i] = luaL_checknumber(L , -1);
        }
    }

    return 0;
}

//% GRADIENT_STOP

LNS_LUA_MEMBER_FUNCTION(gradientStop, _tostring)
{
    LNS_CHECK_AND_GET(L, grad, 1, gradientStop);
    lua_pushfstring(L,
        "<lnanosvg.gradientStop color: [ r:%u g:%u b:%u ] offset:%f> (%p:%p)",
        grad->copy.color & 0xFF, (grad->copy.color >> 8) & 0xFF, (grad->copy.color >> 16) & 0xFF,
        grad->copy.offset,
        grad, grad->ptr
    );
    return 1;
}

LNS_LUA_MEMBER_FUNCTION(gradientStop, _index)
{
    LNS_CHECK_AND_GET(L, grad, 1, gradientStop);
    const char* field = luaL_checkstring(L, 2);

    if (strcmp(field, "color") == 0)
    {
        lua_createtable(L, 4, 0);
        for (int i = 0; i < 3; ++i)
        { // r,g,b
            lua_pushinteger(L, i+1);
            lua_pushinteger(L, (grad->copy.color >> (i * 8)) & 0xFF);
            lua_settable(L, -3);
        }
        // alfa
        lua_pushinteger(L, 4);
        lua_pushinteger(L, 255);
        lua_settable(L, -3);
    }
    else if (strcmp(field, "offset") == 0)
        lua_pushnumber(L, grad->copy.offset);
    else
    {
        lua_getmetatable(L, 1);
        lua_pushvalue(L, 2);
        lua_gettable(L, -2);
    }

    return 1;
}

LNS_LUA_MEMBER_FUNCTION(gradientStop, _newindex)
{
    LNS_CHECK_AND_GET(L, grad, 1, gradientStop);
    const char* field = luaL_checkstring(L, 2);
    unsigned int new_color = 0;

    if (strcmp(field, "color") == 0)
    {
        luaL_checktype(L, 3, LUA_TTABLE);
        for (int i = 0; i < 3; ++i)
        {
            lua_pushinteger(L, i+1);
            lua_gettable(L, -2);
            new_color |= luaL_checkinteger(L, -1) << (i * 8);
        }
        grad->ptr->color = grad->copy.color = new_color;
    }
    else if (strcmp(field, "offset") == 0)
        grad->copy.offset = luaL_checknumber(L, 3);

    return 0;
}

//$------------------------------------------------------------------------------------------------
// REGISTRATION
//#------------------------------------------------------------------------------------------------

#define LNS_MEMBER_FUNC_TABLE_NAME(type) lnanosvg_ ## type ## _funcions
#define LNS_MEMBER_FUNC_TABLE(type) static const struct luaL_Reg LNS_MEMBER_FUNC_TABLE_NAME(type)[]
#define LNS_META_FUNC_TABLE_NAME(type) lnanosvg_ ## type ## _meta
#define LNS_META_FUNC_TABLE(type) static const struct luaL_Reg LNS_META_FUNC_TABLE_NAME(type)[]

LNS_MEMBER_FUNC_TABLE(image) =
{
    {"fromFile",   LNS_MEMBER_FUNC(image, fromFile)  },
    {"fromString", LNS_MEMBER_FUNC(image, fromString)},
    {"iterate",    LNS_MEMBER_FUNC(image, iterate)   },
    {"getCDecl",   LNS_MEMBER_FUNC(image, getCDecl)  },
    {NULL, NULL}
};

LNS_META_FUNC_TABLE(image) =
{
    {"__tostring", LNS_MEMBER_FUNC(image, _tostring)},
    {"__index",    LNS_MEMBER_FUNC(image, _index)   },
    {"__newindex", LNS_MEMBER_FUNC(image, _newindex)},
    {"__gc",       LNS_MEMBER_FUNC(image, _gc)      },
    {NULL, NULL}
};

LNS_MEMBER_FUNC_TABLE(shape) =
{
    {"next",     LNS_MEMBER_FUNC(shape, next)    },
    {"getNext",  LNS_MEMBER_FUNC(shape, getNext) },
    {"iterate",  LNS_MEMBER_FUNC(shape, iterate) },
    {"getCDecl", LNS_MEMBER_FUNC(shape, getCDecl)},
    {NULL, NULL}
};

LNS_META_FUNC_TABLE(shape) =
{
    {"__tostring", LNS_MEMBER_FUNC(shape, _tostring)},
    {"__index",    LNS_MEMBER_FUNC(shape, _index)   },
    {"__newindex", LNS_MEMBER_FUNC(shape, _newindex)},
    {NULL, NULL}
};

LNS_MEMBER_FUNC_TABLE(path) =
{
    {"next",     LNS_MEMBER_FUNC(path, next)    },
    {"getNext",  LNS_MEMBER_FUNC(path, getNext) },
    {"iterate",  LNS_MEMBER_FUNC(path, iterate) },
    {"getCDecl", LNS_MEMBER_FUNC(path, getCDecl)},
    {NULL, NULL}
};

LNS_META_FUNC_TABLE(path) =
{
    {"__tostring", LNS_MEMBER_FUNC(path, _tostring)},
    {"__index",    LNS_MEMBER_FUNC(path, _index)   },
    {"__newindex", LNS_MEMBER_FUNC(path, _newindex)},
    {NULL, NULL}
};

LNS_MEMBER_FUNC_TABLE(paint) =
{
    {"new",        LNS_MEMBER_FUNC(paint, new)       },
    {"toColor",    LNS_MEMBER_FUNC(paint, toColor)   },
    {"toGradient", LNS_MEMBER_FUNC(paint, toGradient)},
    {"getCDecl",   LNS_MEMBER_FUNC(paint, getCDecl)  },
    {NULL, NULL}
};

LNS_META_FUNC_TABLE(paint) =
{
    {"__tostring", LNS_MEMBER_FUNC(paint, _tostring)},
    {"__index",    LNS_MEMBER_FUNC(paint, _index)   },
    {"__newindex", LNS_MEMBER_FUNC(paint, _newindex)},
    {NULL, NULL}
};

LNS_MEMBER_FUNC_TABLE(gradient) =
{
    {"iterate",  LNS_MEMBER_FUNC(path, iterate)     },
    {"getCDecl", LNS_MEMBER_FUNC(gradient, getCDecl)},
    {NULL, NULL}
};

LNS_META_FUNC_TABLE(gradient) =
{
    {"__tostring", LNS_MEMBER_FUNC(gradient, _tostring)},
    {"__index",    LNS_MEMBER_FUNC(gradient, _index)   },
    {"__newindex", LNS_MEMBER_FUNC(gradient, _newindex)},
    {NULL, NULL}
};

LNS_MEMBER_FUNC_TABLE(gradientStop) =
{
    {"toColor",  LNS_MEMBER_FUNC(gradientStop, toColor) },
    {"getCDecl", LNS_MEMBER_FUNC(gradientStop, getCDecl)},
    {NULL, NULL}
};

LNS_META_FUNC_TABLE(gradientStop) =
{
    {"__tostring", LNS_MEMBER_FUNC(gradientStop, _tostring)},
    {"__index",    LNS_MEMBER_FUNC(gradientStop, _index)   },
    {"__newindex", LNS_MEMBER_FUNC(gradientStop, _newindex)},
    {NULL, NULL}
};

//$------------------------------------------------------------------------------------------------
// LIBRARY OPEN
//#------------------------------------------------------------------------------------------------

#define LNS_ADD_TYPE(type)                                                  \
    luaL_newmetatable(L, LNS_LUA_TYPE(type));                               \
    lua_pushvalue(L, -1);                                                   \
    lua_setfield(L, -2, "__index");                                         \
    luaL_register(L, NULL, LNS_META_FUNC_TABLE_NAME(type));                 \
    luaL_register(L, LNS_LUA_TYPE(type), LNS_MEMBER_FUNC_TABLE_NAME(type)); \
    lua_pop(L, 1)

#define LNS_ADD_LENUM(entry)   \
    lua_pushstring(L, #entry); \
    lua_pushvalue(L, -1);      \
    lua_settable(L, -3)

LUALIB_API int luaopen_lnanosvg(lua_State* L)
{

    LNS_ADD_TYPE(image);
    LNS_ADD_TYPE(shape);
    LNS_ADD_TYPE(path);
    LNS_ADD_TYPE(paint);
    LNS_ADD_TYPE(gradient);
    LNS_ADD_TYPE(gradientStop);

    lua_getfield(L, LUA_REGISTRYINDEX, "lnanosvg");
    lua_createtable(L, 0, 8);
    lua_setfield(L, -2, "units");
    lua_getfield(L, -1, "units");
    /**
     *  lnanosvg.units = 
     *  {
     *      ["px"] = "px", ["pt"] = "pt", ["pc"] = "pc",
     *      ["mm"] = "mm", ["cm"] = "cm", ["in"] = "in",
     *      ["em"] = "em", ["ex"] = "ex"
     *  }
     **/
    LNS_ADD_LENUM(px);
    LNS_ADD_LENUM(pt);
    LNS_ADD_LENUM(pc);
    LNS_ADD_LENUM(mm);
    LNS_ADD_LENUM(cm);
    LNS_ADD_LENUM(in);
    LNS_ADD_LENUM(em);
    LNS_ADD_LENUM(ex);
    lua_pop(L,2);

    return 1;
}