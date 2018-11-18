#ifndef LNS_SHAPE_H__
#define LNS_SHAPE_H__

/// LNS
#include "lns_includes.h"
#include "lns_macro.h"

// Member functions
LNS_LUA_MEMBER_FUNCTION(shape, next);
LNS_LUA_MEMBER_FUNCTION(shape, getNext);
LNS_LUA_MEMBER_FUNCTION(shape, iterate);
LNS_LUA_MEMBER_FUNCTION(shape, getCDecl);
// Meta functions
LNS_LUA_MEMBER_FUNCTION(shape, _tostring);
LNS_LUA_MEMBER_FUNCTION(shape, _index);
LNS_LUA_MEMBER_FUNCTION(shape, _newindex);

#endif // LNS_SHAPE_H__