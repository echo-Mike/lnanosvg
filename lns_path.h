#ifndef LNS_PATH_H__
#define LNS_PATH_H__

/// LNS
#include "lns_includes.h"
#include "lns_macro.h"

// Member functions
LNS_LUA_MEMBER_FUNCTION(path, next);
LNS_LUA_MEMBER_FUNCTION(path, getNext);
LNS_LUA_MEMBER_FUNCTION(path, iterate);
LNS_LUA_MEMBER_FUNCTION(path, getCDecl);
// Meta functions
LNS_LUA_MEMBER_FUNCTION(path, _tostring);
LNS_LUA_MEMBER_FUNCTION(path, _index);
LNS_LUA_MEMBER_FUNCTION(path, _newindex);

#endif // LNS_PATH_H__