#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void* create_shared_memory(size_t size) {
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}

// Lua wrapper

#ifndef luaL_newlib
#define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

typedef struct {
    void* memory;
    size_t size;
} shmem_userdata_t;

static int shmem_create(lua_State *L) {
    int size = luaL_checkint(L, 1);
    shmem_userdata_t* shmem_ud;
    shmem_ud = (shmem_userdata_t*) lua_newuserdata(L, sizeof(*shmem_ud));
    shmem_ud->memory = NULL;
    shmem_ud->size = 0;

    luaL_getmetatable(L, "shmem");
    lua_setmetatable(L, -2);
    shmem_ud->memory = create_shared_memory((size_t) size);
    shmem_ud->size = (size_t) size;
    return 1;
}

static int shmem_clear(lua_State *L) {
    shmem_userdata_t* shmem_ud = luaL_checkudata(L, 1, "shmem");
    memset(shmem_ud->memory, 0, shmem_ud->size);
    return 0;
}

static int shmem_write(lua_State *L) {
    shmem_userdata_t* shmem_ud = luaL_checkudata(L, 1, "shmem");
    const char* data = luaL_checkstring(L, 2);
    memcpy(shmem_ud->memory, data, strlen(data));
    return 0;
}

static int shmem_fill(lua_State *L) {
    shmem_userdata_t* shmem_ud = luaL_checkudata(L, 1, "shmem");
    const char* data = luaL_checkstring(L, 2);
    int fill = luaL_optint(L, 3, 0);
    memset(shmem_ud->memory, fill, shmem_ud->size);
    memcpy(shmem_ud->memory, data, strlen(data));
    return 0;
}

static int shmem_read(lua_State *L) {
    shmem_userdata_t* shmem_ud = luaL_checkudata(L, 1, "shmem");
    lua_pushstring(L, shmem_ud->memory);
    return 1;
}

static int shmem_free(lua_State *L) {
    shmem_userdata_t* shmem_ud = luaL_checkudata(L, 1, "shmem");
    int status = munmap(shmem_ud->memory, shmem_ud->size);
    if (status < 0) {
        lua_pushboolean(L, 0);
        lua_pushinteger(L, status);
        return 2;
    } else {
        lua_pushboolean(L, 1);
        return 1;
    }
}

static const struct luaL_Reg shmem_functions[] = {
    { "new", shmem_create },
    { NULL, NULL },
};

static const struct luaL_Reg shmem_methods[] = {
    { "clear", shmem_clear },
    { "write", shmem_write },
    { "fill", shmem_fill },
    { "read", shmem_read },
    { "free", shmem_free },
    { NULL, NULL },
};

int luaopen_shmem(lua_State *L) {
    luaL_newmetatable(L, "shmem");
    // Instance methods
    luaL_newlib(L, shmem_methods);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1); // Remove metatable from stack
    // Library functions
    luaL_newlib(L, shmem_functions);
    return 1;
}
