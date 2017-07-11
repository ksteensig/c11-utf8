#include "utf8.h"

bool is_1byte(uint8_t b) {
    return ONE_BYTE(b);
}

bool is_2byte(uint8_t b) {
    return TWO_BYTE(b);
}

bool is_3byte(uint8_t b) {
    return THREE_BYTE(b);
}

bool is_4byte(uint8_t b) {
    return FOUR_BYTE(b);
}

bool is_trailing(uint8_t b) {
    return TRAILING_BYTE(b);
}

utf8_t *init_utf8() {
    utf8_t *ustr = malloc(sizeof(utf8_t));
    utf8rep_t *ustr_rep = malloc(sizeof(utf8rep_t));
    utf8_cache_t ustr_cache;
    ustr_cache.inserted_length = 0;
    ustr_cache.inserted_pos = 0;

    ustr_rep->utf8_string = malloc(sizeof(uint8_t) * DEFAULT_UTF8_STRING_LENGTH);
    ustr_rep->bytes = 0;
    ustr_rep->chars = 0;
    ustr_rep->cache = ustr_cache;

    ustr->u8str = ustr_rep;
    ustr->error = false;

    return ustr;
}

void append_bytes(utf8_t *ustr, uint8_t bytes[4], uint8_t len) {
    if (ustr->is_error) {
        return;
    }

    switch (len) {
        case 1:
            (ustr->u8str)->utf8_string[ustr->u8str->bytes] = bytes[0];
            ustr->u8str->bytes += len;
            break;
        case 2:
            ustr->u8str->utf8_string[ustr->u8str->bytes] = bytes[1];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 1] = bytes[0];
            ustr->u8str->bytes += len;
            break;
        case 3:
            ustr->u8str->utf8_string[ustr->u8str->bytes] = bytes[2];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 1] = bytes[1];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 2] = bytes[0];
            ustr->u8str->bytes += len;
            break;
        case 4:
            ustr->u8str->utf8_string[ustr->u8str->bytes] = bytes[3];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 1] = bytes[2];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 2] = bytes[1];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 3] = bytes[0];
            ustr->u8str->bytes += len;
            break;
        default:
            ustr->is_error = true;
            ustr->error = INVALID_UTF8;
            return;
    }

    ustr->u8str->chars++;
}

utf8_t *convert_to_utf8(uint8_t *bytes, uint8_t *end_of_array) {
    utf8_t *ustr = init_utf8();

    if (bytes == NULL || end_of_array == NULL) {
        ustr->is_error = true;
        ustr->error = NULL_ERROR;
        return ustr;
    }

    ptrdiff_t diff = end_of_array - bytes;
    uint64_t counter = 0;

    while ((bytes[counter] != '\0' || bytes[counter] != -1) && (counter < diff)) {
        if (is_1byte(bytes[counter])) {
            append_bytes(ustr, bytes, 1);
            counter++;
        } else if (is_2byte(bytes[counter] &&
                   is_trailing(counter + 1))) {
            append_bytes(ustr, bytes, 2);
            counter += 2;
        } else if (is_3byte(bytes[counter]  &&
                   is_trailing(counter + 1) &&
                   is_trailing(counter + 2))) {
            append_bytes(ustr, bytes, 3);
            counter += 3;
        } else if (is_4byte(bytes[counter]  &&
                   is_trailing(counter + 1) &&
                   is_trailing(counter + 2) &&
                   is_trailing(counter + 3))) {
            append_bytes(ustr, bytes, 4);
            counter += 4;
        } else {
            ustr->is_error = true;
            ustr->error = INVALID_UTF8;
        }

        if (ustr->is_error) {
            return ustr;
        }

        counter++;
    }

    return ustr;
}

/*
utf8_t *grow_string(utf8_t *ustr) {
    // double the possible string length
    utf8_t *n_ustr = malloc(sizeof(utf8_t));
    
    n_ustr->utf8_string = malloc(sizeof(uint8_t) * ustr->bytes * 2);
    n_ustr->bytes = ustr->bytes;
    n_ustr->chars = ustr->chars;
    n_ustr->_last_inserted_length = ustr->_last_inserted_length;
    n_ustr->_last_inserted_pos = ustr->_last_inserted_pos;

    memcpy(n_ustr->utf8_string, ustr->utf8_string, sizeof(uint8_t));

    free(ustr->utf8_string);
    free(ustr);

    return n_ustr;
}
*/

/*

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

*/