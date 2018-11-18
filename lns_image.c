#include "lns_constructors.h"

LNS_GEN_STRUCT_DEF_STRING(image);

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

static int lnanosvg_iterate_shapes(lua_State* L)
{
    NSVGshape* p = (NSVGshape*)lua_touserdata(L, 1);
    LNS_CHECK_AND_GET(L, s, 2, shape);
    s->ptr = s->ptr == NULL ? p : s->ptr->next;
    if (s->ptr == NULL)
        lua_pushnil(L);
    return 1;
}

//$------------------------------------------------------------------------------------------------
// CONSTRUCTORS
//#------------------------------------------------------------------------------------------------

LNS_LUA_MEMBER_FUNCTION(image, fromFile)
{
    NSVGimage* img = NULL;
    const char* filename = luaL_checkstring(L, 1);
    const char* units = luaL_checkstring(L, 2);
    float dpi = luaL_checknumber(L, 3);
    
    luaL_argcheck(L, filename != NULL, 1, "file path must be provided");
    luaL_argcheck(L, units != NULL, 2, "units type must be provided");
    luaL_argcheck(L, lnanosvg_units_check(units), 2, "units must have format \"px\"/\"pt\"/\"pc\"/\"mm\"/\"cm\"/\"in\"/\"em\"/\"ex\" or \"%percent_value\"");
    
    lns_init_allocator(L);
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
    lns_init_allocator(L);

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

//$------------------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
//#------------------------------------------------------------------------------------------------

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

//$------------------------------------------------------------------------------------------------
// META FUNCTIONS
//#------------------------------------------------------------------------------------------------

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
    lns_init_allocator(L);
    nsvgDelete(data->ptr);
    memset(data, 0, sizeof(LNS_GET_TYPENAME(image)));
    return 0;
}