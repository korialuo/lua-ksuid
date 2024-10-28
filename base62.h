//
// Created by korialuo on 2024/10/18.
//

#ifndef __base62_h__
#define __base62_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void base62_encode(const uint8_t src[20], uint8_t dst[27]);

void base62_decode(const uint8_t src[27], uint8_t dst[20]);

#ifdef __cplusplus
}
#endif

#endif //__base62_h__
