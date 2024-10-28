//
// Created by korialuo on 2024/10/18.
//
// lua binding for ksuid C.

#define LUA_LIB

#include <lua.h>
#include <lauxlib.h>

#include "ksuid.h"

#define LKSUID_MT ("com.github.korialuo.lksuid")

struct ksuid_ctx {
    int32_t ref;
    void *ud;
    uintptr_t ksuid;
};

static int
lcreate(lua_State *l) {
    int32_t ref = luaL_ref(l, LUA_REGISTRYINDEX);
    struct ksuid_ctx* ctx = (struct ksuid_ctx *)lua_newuserdata(l, sizeof(*ctx));
    assert(ctx);
    ctx->ud = (void *)l;
    ctx->ref = ref;
    ctx->ksuid = ksuid_create();
    assert(ctx->ksuid);

    luaL_getmetatable(l, LKSUID_MT);
    lua_setmetatable(l, -2);

    return 1;
}

static int
ldestroy(lua_State *l) {
    struct ksuid_ctx *ctx = (struct ksuid_ctx *)luaL_checkudata(l, 1, LKSUID_MT);
    if (!ctx)
        return luaL_argerror(l, 1, "parameter self invalid.");
    assert(ctx->ksuid);
    luaL_unref(l, LUA_REGISTRYINDEX, ctx->ref);
    ksuid_destroy(ctx->ksuid);
    ctx->ksuid = 0;
    ctx->ref = 0;

    return 0;
}

static int
lnext(lua_State *l) {
    struct ksuid_ctx *ctx = (struct ksuid_ctx *)luaL_checkudata(l, 1, LKSUID_MT);
    if (!ctx)
        return luaL_argerror(l, 1, "parameter self invalid.");
    assert(ctx->ksuid);

    if (ksuid_gen(ctx->ksuid))
        return luaL_error(l, "gen ksuid error.");

    const char *ksuid_str = ksuid_get_string(ctx->ksuid);
    lua_pushstring(l, ksuid_str);

    return 1;
}


LUAMOD_API int
luaopen_lksuid(lua_State *l) {
    luaL_checkversion(l);

    luaL_Reg lib[] {
        {"create", lcreate},
        {NULL, NULL},
    };

    luaL_Reg lib2[] = {
        {"__gc", ldestroy},
        {"next", lnext},
        {NULL, NULL},
    };

    if (luaL_newmetatable(l, LKSUID_MT)) {
        lua_pushvalue(l, -1);
        lua_setfield(l, -2, "__index");
        luaL_setfuncs(l, lib2, 0);
        lua_pop(l, 1);
    }

    lua_newlib(l, lib);

    return 1;
}