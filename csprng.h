//
// Created by korialuo on 2024/10/18.
//

#ifndef __csprng_h__
#define __csprng_h__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t csprng_create();

size_t csprng_get(uintptr_t ctx, void *dst, size_t length);

int32_t csprng_get_int32(uintptr_t ctx);

int64_t csprng_get_int64(uintptr_t ctx);

uint32_t csprng_get_uint32(uintptr_t ctx);

uint64_t csprng_get_uint64(uintptr_t ctx);

void csprng_destroy(uintptr_t ctx);

#ifdef __cplusplus
}
#endif

#endif // __csprng_h__