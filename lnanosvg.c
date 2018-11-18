#include "lnanosvg.h"

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