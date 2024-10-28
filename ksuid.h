//
// Created by korialuo on 2024/10/18.
//

#ifndef __ksuid_h__
#define __ksuid_h__

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t ksuid_create();

int ksuid_gen_with_time(uintptr_t ctx, time_t time);

int ksuid_gen(uintptr_t ctx);

const char* ksuid_get_string(uintptr_t ctx);

void ksuid_destroy(uintptr_t ctx);

#ifdef __cplusplus
}
#endif

#endif // __ksuid_h__