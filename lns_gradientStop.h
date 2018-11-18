#ifndef LNS_GRADIENT_STOP_H__
#define LNS_GRADIENT_STOP_H__

/// LNS
#include "lns_includes.h"
#include "lns_macro.h"

// Member functions
LNS_LUA_MEMBER_FUNCTION(gradientStop, toColor);
LNS_LUA_MEMBER_FUNCTION(gradientStop, getCDecl);
// Meta functions
LNS_LUA_MEMBER_FUNCTION(gradientStop, _tostring);
LNS_LUA_MEMBER_FUNCTION(gradientStop, _index);
LNS_LUA_MEMBER_FUNCTION(gradientStop, _newindex);

#endif // LNS_GRADIENT_STOP_H__