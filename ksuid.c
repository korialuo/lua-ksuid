//
// Created by korialuo on 2024/10/18.
//

#include "ksuid.h"
#include "base62.h"
#include "csprng.h"

#include <stdlib.h>
#include <time.h>


// Timestamp is a uint32
#define TIMESTAMP_LENGTH_IN_BYTES 4

// Payload is 16-bytes
#define PAYLOAD_LENGTH_IN_BYTES 16

// KSUIDs are 20 bytes when binary encoded
#define BYTE_LENGTH  (TIMESTAMP_LENGTH_IN_BYTES + PAYLOAD_LENGTH_IN_BYTES)

// The length of a KSUID when string (base62) encoded
#define STRING_ENCODE_LENGTH 27

// KSUID's epoch starts more recently so that the 32-bit number space gives a
// significantly higher useful lifetime of around 136 years from March 2017.
// This number (14e8) was picked to be easy to remember.
static const uint64_t EPOCH_STAMP = 1400000000;

// A string-encoded minimum value for a KSUID
// static const char *MIN_STRING_ENCODED = "000000000000000000000000000";

// A string-encoded maximum value for a KSUID
// static const char *MAX_STRING_ENCODED = "aWgEPTl1tmebfsQzFP4bxwgy80V";


typedef struct {
    uint8_t ksuid[BYTE_LENGTH];
    char ksuid_str[STRING_ENCODE_LENGTH+1];
    uintptr_t csprng;
} ksuid_t;


// create a ksuid context
uintptr_t ksuid_create() {
    uintptr_t csprng = csprng_create();
    if (!csprng) {
        return 0;
    }
    ksuid_t *c = (ksuid_t *)malloc(sizeof(*c));
    c->csprng = csprng;
    return (uintptr_t)c;
}


// generate a ksuid with time stamp
int ksuid_gen_with_time(uintptr_t ctx, time_t time) {
    // time stamp from a specifie epoch
    uint32_t stamp = (uint32_t)((uint64_t)time - EPOCH_STAMP);
    ksuid_t *c = (ksuid_t *)ctx;
    // big endian
    c->ksuid[0] = (uint8_t)((stamp >> 24) & 0xff);
    c->ksuid[1] = (uint8_t)((stamp >> 16) & 0xff);
    c->ksuid[2] = (uint8_t)((stamp >> 8) & 0xff);
    c->ksuid[3] = (uint8_t)(stamp & 0xff);

    if (csprng_get(
            c->csprng,
            c->ksuid + TIMESTAMP_LENGTH_IN_BYTES,
            PAYLOAD_LENGTH_IN_BYTES
            ) != PAYLOAD_LENGTH_IN_BYTES) {
        return -1;
    }
    base62_encode(c->ksuid, (uint8_t *)c->ksuid_str);
    c->ksuid_str[STRING_ENCODE_LENGTH] = '\0';
    return 0;
}

inline int ksuid_gen(uintptr_t ctx) {
    return ksuid_gen_with_time(ctx, time(NULL));
}

const char* ksuid_get_string(uintptr_t ctx) {
    ksuid_t *c = (ksuid_t *)ctx;
    return (const char *)c->ksuid_str;
}

void ksuid_destroy(uintptr_t ctx) {
    ksuid_t *c = (ksuid_t *)ctx;
    csprng_destroy(c->csprng);
    free(c);
}