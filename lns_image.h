#ifndef LNS_IMAGE_H__
#define LNS_IMAGE_H__

/// LNS
#include "lns_includes.h"
#include "lns_macro.h"

/// Constructors
LNS_LUA_MEMBER_FUNCTION(image, fromFile);
LNS_LUA_MEMBER_FUNCTION(image, fromString);
// Member functions
LNS_LUA_MEMBER_FUNCTION(image, iterate);
LNS_LUA_MEMBER_FUNCTION(image, getCDecl);
// Meta functions
LNS_LUA_MEMBER_FUNCTION(image, _tostring);
LNS_LUA_MEMBER_FUNCTION(image, _index);
LNS_LUA_MEMBER_FUNCTION(image, _newindex);
LNS_LUA_MEMBER_FUNCTION(image, _gc);

#endif // LNS_IMAGE_H__