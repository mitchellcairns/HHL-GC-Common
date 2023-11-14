#ifndef UTIL_COMMON_H
#define UTIL_COMMON_H

#include <stdbool.h>
#include <stdint.h>

#define CLAMP_0_255(value) ((value) < 0 ? 0 : ((value) > 255 ? 255 : (value)))

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

bool util_getbit(uint32_t bytes, uint8_t bit);

#endif