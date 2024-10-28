//
// Created by korialuo on 2024/10/18.
//

#include "base62.h"
#include <string.h>

#define OFFSET_UPPERCASE 10
#define OFFSET_LOWERCASE 36

#define __bytes2uint32_bigendian(x) (uint32_t)( ((uint32_t)(x)[0] << 24) | ((uint32_t)(x)[1] << 16) | ((uint32_t)(x)[2] << 8) | ((uint32_t)(x)[3]) )

static const char *zero_string = "000000000000000000000000000";
static const char *base62_characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


// Converts a base 62 byte into the number value that it represents.
static uint8_t base62_value(uint8_t digit) {
    if (digit >= '0' && digit <= '9') {
        return digit - '0';
    }
    else if (digit >= 'A' && digit <= 'Z') {
        return OFFSET_UPPERCASE + (digit - 'A');
    }
    return OFFSET_LOWERCASE + (digit - 'a');
}

// This function encodes the base 62 representation of the src KSUID in binary
// form into dst.
//
// In order to support a couple of optimizations the function assumes that src
// is 20 bytes long and dst is 27 bytes long.
//
// Any unused bytes in dst will be set to the padding '0' byte.
void base62_encode(const uint8_t src[20], uint8_t dst[27]) {
    static const uint64_t src_base = 4294967296;
    static const uint64_t dst_base = 62;

    // Split src into 5 4-byte words, this is where most of the efficiency comes
    // from because this is a O(N^2) algorithm, and we make N = N / 4 by working
    // on 32 bits at a time.
    uint32_t parts[5] = {
            __bytes2uint32_bigendian(src),
            __bytes2uint32_bigendian(src+4),
            __bytes2uint32_bigendian(src+8),
            __bytes2uint32_bigendian(src+12),
            __bytes2uint32_bigendian(src+16)
    };
    size_t n = 27;
    uint32_t *bp = parts;
    size_t bp_len = 5;
    uint32_t bq[5] = {0};

    for (;bp_len != 0;) {
        uint32_t *quotient = bq;
        size_t quotient_offset = 0;
        uint64_t remainder = 0;

        for (int i = 0; i < bp_len; i++) {
            uint64_t c = (uint64_t)bp[i];
            uint64_t value = c + remainder * src_base;
            uint64_t digit = value / dst_base;
            remainder = value % dst_base;

            if (quotient_offset != 0 || digit != 0) {
                quotient[quotient_offset++] = (uint32_t)digit;
            }
        }

        // Writes at the end of the destination buffer because we computed the
        // lowest bits first.
        dst[--n] = base62_characters[remainder];
        bp = quotient;
        bp_len = quotient_offset;
    }

    // Add padding at the head of the destination buffer for all bytes that were
    // not set.
    strncpy((char *)dst, zero_string, n);
}

// This function decodes the base 62 representation of the src KSUID to the
// binary form into dst.
//
// In order to support a couple of optimizations the function assumes that src
// is 27 bytes long and dst is 20 bytes long.
//
// Any unused bytes in dst will be set to zero.
void base62_decode(const uint8_t src[27], uint8_t dst[20]) {
    static const uint64_t src_base = 62;
    static const uint64_t dst_base = 4294967296;

    uint8_t parts[27] = {
            base62_value(src[0]),
            base62_value(src[1]),
            base62_value(src[2]),
            base62_value(src[3]),
            base62_value(src[4]),
            base62_value(src[5]),
            base62_value(src[6]),
            base62_value(src[7]),
            base62_value(src[8]),
            base62_value(src[9]),
            base62_value(src[10]),
            base62_value(src[11]),
            base62_value(src[12]),
            base62_value(src[13]),
            base62_value(src[14]),
            base62_value(src[15]),
            base62_value(src[16]),
            base62_value(src[17]),
            base62_value(src[18]),
            base62_value(src[19]),
            base62_value(src[20]),
            base62_value(src[21]),
            base62_value(src[22]),
            base62_value(src[23]),
            base62_value(src[24]),
            base62_value(src[25]),
            base62_value(src[26])
    };

    size_t n = 20;
    uint8_t *bp = parts;
    size_t bp_len = 27;
    uint8_t bq[27] = {0};

    for (;bp_len > 0;) {
        uint8_t *quotient = bq;
        size_t quotient_offset = 0;
        uint64_t remainder = 0;

        for (int i = 0; i < bp_len; i++) {
            uint64_t c = bp[i];
            uint64_t value = c + remainder * src_base;
            uint64_t digit = value / dst_base;
            remainder = value % dst_base;

            if (quotient_offset != 0 || digit != 0) {
                quotient[quotient_offset++] = (uint8_t)digit;
            }
        }

        dst[n-4] = (uint8_t)(remainder >> 24);
        dst[n-3] = (uint8_t)(remainder >> 16);
        dst[n-2] = (uint8_t)(remainder >> 8);
        dst[n-1] = (uint8_t)remainder;
        n -= 4;
        bp = quotient;
        bp_len = quotient_offset;
    }

    memset(dst, 0, n);
}