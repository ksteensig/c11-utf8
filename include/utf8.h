#pragma once

#include <stdint.h>
#include <stdbool.h>

#define FOUR_BYTE_HEAD 0b11110
#define FOUR_BYTE(c) ((c >> 27) ^ FOUR_BYTE_HEAD)

#define THREE_BYTE_HEAD 0b1110
#define THREE_BYTE(c) (((c >> 20) & 0xF) ^ THREE_BYTE_HEAD)

#define TWO_BYTE_HEAD 0b110
#define TWO_BYTE(c) (((c >> 13) & 0x7) ^ TWO_BYTE_HEAD)

#define ONE_BYTE_HEAD 0b0
#define ONE_BYTE(c) (((0b0 >> 7) & 0x0) ^ ONE_BYTE_HEAD)
#define FOLLOW_BYTE_HEAD(c, s) (((c >> s) & 0xFF) ^ 0b10)
#define ZERO(c, s) (c >> s)

bool is_1byte(uint32_t ch);
bool is_2byte(uint32_t ch);
bool is_3byte(uint32_t ch);
bool is_4byte(uint32_t ch);

typedef struct utf8str_s {
    uint8_t *utf8_string;
    uint64_t arr_len;
    uint64_t chars;
    uint64_t last_inserted_pos;
    uint8_t last_inserted_length;
} utf8str_t;

typedef struct utf8_s {
    uint8_t bytes;
    uint8_t uchar[4];
} utf8_t;

utf8_t convert_to_utf8(uint32_t ch);

utf8str_t utf8_concat(utf8str_t ustr1, utf8str_t ustr2);
utf8str_t utf8_append(utf8str_t ustr, utf8_t uchar);
utf8str_t utf8_prepend(utf8str_t ustr, utf8_t uchar);