//
// Created by korialuo on 2024/10/18.
//

#include "csprng.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__)
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "advapi32.lib")

typedef struct {
    HCRYPTPROV hCryptProv;
} csprng_t;

uintptr_t csprng_create() {
    csprng_t *c = (csprng_t *) malloc(sizeof(*c));
    if (!CryptAcquireContextA(
            &c->hCryptProv,
            NULL,
            NULL,
            PROV_RSA_FULL,
            CRYPT_VERIFYCONTEXT | CRYPT_SILENT)
            ) {
        free(c);
        return 0;
    }
    return (uintptr_t)c;
}


size_t csprng_get(uintptr_t ctx, void *dst, size_t length) {
    csprng_t *c = (csprng_t *)ctx;
    uint32_t u;
    size_t offset = 0;
    while (offset < length) {
        if (!CryptGenRandom(
                c->hCryptProv,
                sizeof(u),
                (BYTE *)&u)
           ) {
            return 0;
        }
        memcpy_s(dst + offset, length, (void *)&u, sizeof(u));
        offset += (length < sizeof(u) ? length : sizeof(u));
    }
    return offset;
}

void csprng_destroy(uintptr_t ctx) {
    csprng_t *c = (csprng_t *)ctx;
    CryptReleaseContext(c->hCryptProv, 0);
    free(c);
}

#else

typedef struct {
    FILE *fd;
} csprng_t;

uintptr_t csprng_create() {
    csprng_t *c = (csprng_t *)malloc(sizeof(*c));
    c->fd = fopen("/dev/urandom", "rb");
    if (!c->fd) {
        return 0;
    }
    return (uintptr_t)c;
}

size_t csprng_get(uintptr_t ctx, void *dst, size_t length) {
    csprng_t *c = (csprng_t *)ctx;
    return fread(dst, sizeof(uint8_t), length, c->fd);
}

void csprng_destroy(uintptr_t ctx) {
    csprng_t *c = (csprng_t *)ctx;
    fclose(c->fd);
    free(c);
}

#endif


int32_t csprng_get_int32(uintptr_t ctx) {
    int32_t i;
    if (csprng_get(ctx, &i, sizeof(i)) != sizeof(i)) {
        return 0;
    }
    return i;
}

int64_t csprng_get_int64(uintptr_t ctx) {
    int64_t i;
    if (csprng_get(ctx, &i, sizeof(i)) != sizeof(i)) {
        return 0;
    }
    return i;
}

uint32_t csprng_get_uint32(uintptr_t ctx) {
    uint32_t i;
    if (csprng_get(ctx, &i, sizeof(i)) != sizeof(i)) {
        return 0;
    }
    return i;
}

uint64_t csprng_get_uint64(uintptr_t ctx) {
    uint64_t i;
    if (csprng_get(ctx, &i, sizeof(i)) != sizeof(i)) {
        return 0;
    }
    return i;
}