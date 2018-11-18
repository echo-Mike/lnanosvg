#include "lns_constructors.h"

LNS_GEN_STRUCT_DEF_STRING(shape);

//$------------------------------------------------------------------------------------------------
// FREE FUNCTIONS
//#------------------------------------------------------------------------------------------------

static int lnanosvg_iterate_paths(lua_State* L)
{
    NSVGpath* init_ptr = (NSVGpath*)lua_touserdata(L, 1);
    LNS_CHECK_AND_GET(L, p, 2, path);
    p->ptr = p->ptr == NULL ? init_ptr : p->ptr->next;
    if (p->ptr == NULL)
        lua_pushnil(L);
    return 1;
}

//$------------------------------------------------------------------------------------------------
// CONSTRUCTORS
//#------------------------------------------------------------------------------------------------

LNS_CONSTRUCTOR_ITER(shape, NSVGshape)

//$------------------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
//#------------------------------------------------------------------------------------------------

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

//$------------------------------------------------------------------------------------------------
// META FUNCTIONS
//#------------------------------------------------------------------------------------------------

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