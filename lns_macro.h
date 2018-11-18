#ifndef LNS_MACRO_H__
#define LNS_MACRO_H__

//$------------------------------------------------------------------------------------------------
// GENERAL
//#------------------------------------------------------------------------------------------------

/**
#if defined(__alignof_is_defined)
#   define LNS_ALIGNOF(type) alignof(type)
#else
#   define LNS_ALIGNOF(type) offsetof (struct { char c; type member; }, member)
#endif
**/

#define LNS_DEFER_(x) x

#define LNS_STRINGIFY_(x) #x
#define LNS_STRINGIFY(x) LNS_STRINGIFY_(x)

#ifdef __cplusplus
#   define LNS_EXTERN_C extern "C" {
#   define LNS_EXTERN_C_END }
#else 
#   define LNS_EXTERN_C
#   define LNS_EXTERN_C_END
#endif

#if defined(__STDC_LIB_EXT1__)
#   define LNS_STRLEN(str, size) strnlen_s(str, (size))
#else
#   define LNS_STRLEN(str, size) strlen(str)
#endif

//$------------------------------------------------------------------------------------------------
// TYPES
//#------------------------------------------------------------------------------------------------

#define LNS_GET_TYPENAME(name) NSVG_ ## name ## _udata

#define LNS_GET_STRUCT_STR(type) NSVG_struct_ ## type

#define LNS_LUA_TYPE(type) "lnanosvg." #type

//$------------------------------------------------------------------------------------------------
// GENERATORS
//#------------------------------------------------------------------------------------------------

#define LNS_GEN_STRUCT(name)             \
    typedef struct NSVG_##name##_udata_t \
    {                                    \
        NSVG##name copy;                 \
        NSVG##name *ptr;                 \
    } NSVG_##name##_udata

#define LNS_GEN_STRUCT_DEF_STRING(name) \
    static const char NSVG_struct_##name[] = LNS_STRINGIFY(LNS_GEN_STRUCT_(name))

#define LNS_GEN_ITER_STRUCT(name)        \
    typedef struct NSVG_##name##_udata_t \
    {                                    \
        NSVG##name *ptr;                 \
    } NSVG_##name##_udata

#define LNS_GEN_ITER_STRUCT_DEF_STRING(name) \
    static const char NSVG_struct_##name[] = LNS_STRINGIFY(LNS_GEN_ITER_STRUCT_(name))

//$------------------------------------------------------------------------------------------------
// LUA FUNCTION WRAPPERS
//#------------------------------------------------------------------------------------------------

// Calls luaL_checkudata with specific wrapper type
#define check_nsvgimage(L, n)        (LNS_GET_TYPENAME(image)*)luaL_checkudata(L, n, LNS_LUA_TYPE(image))
#define check_nsvgshape(L, n)        (LNS_GET_TYPENAME(shape)*)luaL_checkudata(L, n, LNS_LUA_TYPE(shape))
#define check_nsvgpath(L, n)         (LNS_GET_TYPENAME(path)*)luaL_checkudata(L, n, LNS_LUA_TYPE(path))
#define check_nsvgpaint(L, n)        (LNS_GET_TYPENAME(paint)*)luaL_checkudata(L, n, LNS_LUA_TYPE(paint))
#define check_nsvggradient(L, n)     (LNS_GET_TYPENAME(gradient)*)luaL_checkudata(L, n, LNS_LUA_TYPE(gradient))
#define check_nsvggradientStop(L, n) (LNS_GET_TYPENAME(gradientStop)*)luaL_checkudata(L, n, LNS_LUA_TYPE(gradientStop))

// Declares variable with name 'varname' and type pointer to 'type' and obtains it from lua argument 'pos' with error check
#define LNS_CHECK_AND_GET(L, varname, pos, type) \
    LNS_GET_TYPENAME(type) *varname = LNS_DEFER_(LNS_DEFER_(check_nsvg##type)(L, pos))

// Allocates new userdata with memory sutable for type 'type' plus declares variable with name 'varname' and type pointer to 'type'
#define LNS_LALLOC_UDATA(L, varname, type) \
    LNS_GET_TYPENAME(type) *varname = (LNS_GET_TYPENAME(type) *)lua_newuserdata(L, sizeof(LNS_GET_TYPENAME(type)))

//$------------------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
//#------------------------------------------------------------------------------------------------

// Emits name for C function as member function 'funcname' of type 'type'
#define LNS_MEMBER_FUNC(type, funcname) lnanosvg_ ## type ## _ ## funcname
// Emits declaration of function of type lua_CFunction with name emitted by LNS_MEMBER_FUNC macro
#define LNS_LUA_MEMBER_FUNCTION(type, funcname) int LNS_MEMBER_FUNC(type, funcname)(lua_State* L)

// Emits definition of constructor for type defined with LNS_GEN_STRUCT macro
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

// Emits definition of constructor for type defined with LNS_GEN_ITER_STRUCT macro
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

#endif // LNS_MACRO_H__