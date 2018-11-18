#include "lns_constructors.h"

LNS_GEN_STRUCT_DEF_STRING(paint);

//$------------------------------------------------------------------------------------------------
// CONSTRUCTORS
//#------------------------------------------------------------------------------------------------

LNS_CONSTRUCTOR(paint, NSVGpaint)

//$------------------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
//#------------------------------------------------------------------------------------------------

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

//$------------------------------------------------------------------------------------------------
// META FUNCTIONS
//#------------------------------------------------------------------------------------------------

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