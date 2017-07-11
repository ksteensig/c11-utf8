#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_UTF8_STRING_LENGTH 4

#define FOUR_BYTE_HEAD 0b11110
#define FOUR_BYTE(c) (((c >> 3) ^ FOUR_BYTE_HEAD) == 0)

#define THREE_BYTE_HEAD 0b1110
#define THREE_BYTE(c) (((c >> 4) ^ THREE_BYTE_HEAD) == 0)

#define TWO_BYTE_HEAD 0b110
#define TWO_BYTE(c) (((c >> 5) ^ TWO_BYTE_HEAD) == 0)

#define ONE_BYTE_HEAD 0b0
#define ONE_BYTE(c) (((c >> 7) & ONE_BYTE_HEAD) == 0)

#define TRAILING_BYTE_HEAD 0b10
#define TRAILING_BYTE(c) (((c >> 6) ^ 0b10) == 0)

bool is_1byte(uint8_t b);
bool is_2byte(uint8_t b);
bool is_3byte(uint8_t b);
bool is_4byte(uint8_t b);
bool is_trailing(uint8_t b);

typedef enum utf8_error_e {
    INVALID_UTF8        = 0,
    OUT_OF_BOUND_ERROR  = 1,
    NULL_ERROR          = 2,
    UNKNOWN             = 0xFF
} utf8_error_t;

// cache for storing latest insertion position and length of character inserted in an utf8 string
typedef struct utf8_cache_s {
    uint64_t inserted_pos;  // position of first byte
    uint8_t inserted_length;// length of inserted utf8 character
} utf8_cache_t;

// utf8 representation
typedef struct utf8rep_s {
    uint8_t *utf8_string;   // the utf8 string represented by a uint8_t array
    uint64_t bytes;         // bytes occupied in the utf8_string
    uint64_t length;        // length of utf8_string array
    uint64_t chars;         // char count
    utf8_cache_t cache;     // contains where the last char was inserted and how many uint8_t it is
} utf8rep_t;

// wrapper type which can be an error or a utf8 string
typedef struct utf8_s {
    bool is_error; // false means it's a utf8rep_t
    utf8_error_t error;
    utf8rep_t *u8str;
} utf8_t;

utf8_t *init_utf8();
void free_utf8rep(utf8_t *ustr);
void free_utf8(utf8_t *ustr);

void grow_string(utf8_t *ustr);
utf8_t *convert_to_utf8(uint8_t *bytes, uint64_t len);
void add_character(utf8_t *ustr, uint8_t bytes[4], uint8_t len);

/*
utf8_t *utf8_concat(utf8_t ustr1, utf8_t ustr2);
utf8_t *utf8_append(utf8_t ustr1, utf8_t ustr2);
utf8_t *utf8_prepend(utf8_t ustr1, utf8_t ustr2);
utf8_t *utf8_insert(utf8_t ustr, _utf8_t uchar, uint64_t pos);
utf8_t *utf8_remove(utf8_t ustr, uint64_t pos);
utf8_t *utf8_overwrite(utf8_t ustr, uint64_t pos);
*/
void utf8_print(utf8_t ustr);
utf8_t utf8_read();