CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

project(lua-ksuid LANGUAGES C)

add_library(lua-ksuid SHARED
        base62.c
        csprng.c
        ksuid.c)