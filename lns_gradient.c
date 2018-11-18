#include "lns_constructors.h"

LNS_GEN_STRUCT_DEF_STRING(gradient);

typedef struct LNS_GradStopIterator_t
{
    int nstops, current;
    NSVGgradientStop* begin;
} LNS_GradStopIterator;

//$------------------------------------------------------------------------------------------------
// FREE FUNCTIONS
//#------------------------------------------------------------------------------------------------

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

//$------------------------------------------------------------------------------------------------
// CONSTRUCTORS
//#------------------------------------------------------------------------------------------------

LNS_CONSTRUCTOR(gradient, NSVGgradient)

//$------------------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
//#------------------------------------------------------------------------------------------------

LNS_LUA_MEMBER_FUNCTION(gradient, iterate)
{
    LNS_CHECK_AND_GET(L, grad, 1, gradient);
    lua_pushcfunction(L, lnanosvg_iterate_gradstop);
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

//$------------------------------------------------------------------------------------------------
// META FUNCTIONS
//#------------------------------------------------------------------------------------------------

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