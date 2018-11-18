#include "lns_constructors.h"

LNS_GEN_STRUCT_DEF_STRING(gradientStop);

//$------------------------------------------------------------------------------------------------
// CONSTRUCTORS
//#------------------------------------------------------------------------------------------------

LNS_CONSTRUCTOR(gradientStop, NSVGgradientStop)

//$------------------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
//#------------------------------------------------------------------------------------------------

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
// META FUNCTIONS
//#------------------------------------------------------------------------------------------------

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