#ifndef LNS_GRADIENT_H__
#define LNS_GRADIENT_H__

/// LNS
#include "lns_includes.h"
#include "lns_macro.h"

// Member functions
LNS_LUA_MEMBER_FUNCTION(gradient, iterate);
LNS_LUA_MEMBER_FUNCTION(gradient, getCDecl);
// Meta functions
LNS_LUA_MEMBER_FUNCTION(gradient, _tostring);
LNS_LUA_MEMBER_FUNCTION(gradient, _index);
LNS_LUA_MEMBER_FUNCTION(gradient, _newindex);

#endif // LNS_GRADIENT_H__