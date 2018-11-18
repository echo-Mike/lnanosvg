#ifndef LNS_PAINT_H__
#define LNS_PAINT_H__

/// LNS
#include "lns_includes.h"
#include "lns_macro.h"

// Member functions
LNS_LUA_MEMBER_FUNCTION(paint, toColor);
LNS_LUA_MEMBER_FUNCTION(paint, toGradient);
LNS_LUA_MEMBER_FUNCTION(paint, getCDecl);
// Meta functions
LNS_LUA_MEMBER_FUNCTION(paint, _tostring);
LNS_LUA_MEMBER_FUNCTION(paint, _index);
LNS_LUA_MEMBER_FUNCTION(paint, _newindex);

#endif // LNS_PAINT_H__