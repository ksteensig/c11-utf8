#include "utf8.h"

bool is_1byte(uint32_t ch) {
    return (ONE_BYTE(ch) == 0) &&
           (ZERO(ch, 24) == 0) &&
           (ZERO(ch, 16) == 0) &&
           (ZERO(ch, 8) == 0);
}

bool is_2byte(uint32_t ch) {
    return (TWO_BYTE(ch) == 0) &&
           (ZERO(ch, 24) == 0) &&
           (ZERO(ch, 16) == 0) &&
           (FOLLOW_BYTE_HEAD(ch, 0) == 0);
}

bool is_3byte(uint32_t ch) {
    return (THREE_BYTE(ch) == 0) &&
           (ZERO(ch, 24) == 0) &&
           (FOLLOW_BYTE_HEAD(ch, 8) == 0) &&
           (FOLLOW_BYTE_HEAD(ch, 0) == 0);
}

bool is_4byte(uint32_t ch) {
    return (FOUR_BYTE(ch) == 0) &&
           (FOLLOW_BYTE_HEAD(ch, 16) == 0) &&
           (FOLLOW_BYTE_HEAD(ch, 8) == 0) &&
           (FOLLOW_BYTE_HEAD(ch, 0) == 0);
}

utf8_t convert_to_utf8(uint32_t ch) {
    utf8_t u;

    if (is_1byte(ch)) {
        u.bytes = 1;
        u.uchar[0] = (uint8_t)(ch & 0xFF);
    } else if (is_2byte(ch)) {
        u.bytes = 2;
        u.uchar[0] = (uint8_t)(ch & 0xFF);
        u.uchar[1] = (uint8_t)((ch >> 8) & 0xFF);
    } else if (is_3byte(ch)) {
        u.bytes = 3;
        u.uchar[0] = (uint8_t)(ch & 0xFF);
        u.uchar[1] = (uint8_t)((ch >> 8) & 0xFF);
        u.uchar[2] = (uint8_t)((ch >> 16) & 0xFF);
    } else if (is_4byte(ch)) {
        u.bytes = 4;
        u.uchar[0] = (uint8_t)(ch & 0xFF);
        u.uchar[1] = (uint8_t)((ch >> 8) & 0xFF);
        u.uchar[2] = (uint8_t)((ch >> 16) & 0xFF);
        u.uchar[3] = (uint8_t)((ch >> 24) & 0xFF);
    } else {
        u.bytes = 0;
    }

    return u;
}