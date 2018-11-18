#include "lns_constructors.h"

LNS_GEN_STRUCT_DEF_STRING(path);

typedef struct LNS_DotIterator_t
{
    int npts, current;
    float* begin;
} LNS_DotIterator;

//$------------------------------------------------------------------------------------------------
// FREE FUNCTIONS
//#------------------------------------------------------------------------------------------------

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

//$------------------------------------------------------------------------------------------------
// CONSTRUCTORS
//#------------------------------------------------------------------------------------------------

LNS_CONSTRUCTOR_ITER(path, NSVGpath)

//$------------------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
//#------------------------------------------------------------------------------------------------

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

//$------------------------------------------------------------------------------------------------
// META FUNCTIONS
//#------------------------------------------------------------------------------------------------

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