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
    ustr_rep->length = DEFAULT_UTF8_STRING_LENGTH;
    ustr_rep->bytes = 0;
    ustr_rep->chars = 0;
    ustr_rep->cache = ustr_cache;

    ustr->is_error = false;
    ustr->u8str = ustr_rep;

    return ustr;
}

void append_bytes(utf8_t *ustr, uint8_t bytes[4], uint8_t len) {
    
    if (ustr->is_error) {
        return;
    } else if ((ustr->u8str->length - ustr->u8str->bytes) <= 4) {
        grow_string(ustr);
    }

    switch (len) {
        case 1:
            ustr->u8str->utf8_string[ustr->u8str->bytes] = bytes[0];
            ustr->u8str->bytes += len;
            break;
        case 2:
            ustr->u8str->utf8_string[ustr->u8str->bytes] = bytes[0];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 1] = bytes[1];
            ustr->u8str->bytes += len;
            break;
        case 3:
            ustr->u8str->utf8_string[ustr->u8str->bytes] = bytes[0];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 1] = bytes[1];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 2] = bytes[2];
            ustr->u8str->bytes += len;
            break;
        case 4:
            ustr->u8str->utf8_string[ustr->u8str->bytes] = bytes[0];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 1] = bytes[1];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 2] = bytes[2];
            ustr->u8str->utf8_string[ustr->u8str->bytes + 3] = bytes[3];
            ustr->u8str->bytes += len;
            break;
        default:
            ustr->is_error = true;
            ustr->error = INVALID_UTF8;
            return;
    }

    ustr->u8str->chars++;
}

utf8_t *convert_to_utf8(uint8_t *bytes, uint64_t len) {
    utf8_t *ustr = init_utf8();

    if (bytes == NULL) {
        ustr->is_error = true;
        ustr->error = NULL_ERROR;
        return ustr;
    }

    uint8_t nul[] = {'\0'};

    uint64_t counter = 0;

    while (bytes[counter] != '\0' && bytes[counter] != -1 && (counter < len)) {
        if (is_1byte(bytes[counter])) {
            append_bytes(ustr, bytes + counter, 1);
            counter++;
        } else if (is_2byte(bytes[counter] &&
                   is_trailing(counter + 1))) {
            append_bytes(ustr, bytes + counter, 2);
            counter += 2;
        } else if (is_3byte(bytes[counter]  &&
                   is_trailing(counter + 1) &&
                   is_trailing(counter + 2))) {
            append_bytes(ustr, bytes + counter, 3);
            counter += 3;
        } else if (is_4byte(bytes[counter]  &&
                   is_trailing(counter + 1) &&
                   is_trailing(counter + 2) &&
                   is_trailing(counter + 3))) {
            append_bytes(ustr, bytes + counter, 4);
            counter += 4;
        } else {
            ustr->is_error = true;
            ustr->error = INVALID_UTF8;
            return ustr;
        }
    }

    append_bytes(ustr, nul, 1);

    return ustr;
}

void grow_string(utf8_t *ustr) {
    // double the possible string length
    utf8rep_t *n_ustr = malloc(sizeof(utf8rep_t));
    
    n_ustr->utf8_string = malloc(sizeof(uint8_t) * ustr->u8str->length * 2);
    n_ustr->length = ustr->u8str->length * 2; 
    n_ustr->bytes = ustr->u8str->bytes;
    n_ustr->chars = ustr->u8str->chars;
    n_ustr->cache.inserted_length = ustr->u8str->cache.inserted_length;
    n_ustr->cache.inserted_pos = ustr->u8str->cache.inserted_pos;

    memcpy(n_ustr->utf8_string, ustr->u8str->utf8_string, sizeof(uint8_t) * ustr->u8str->bytes);

    free_utf8rep(ustr);

    ustr->u8str = n_ustr;
}

void free_utf8rep(utf8_t *ustr) {
    free(ustr->u8str->utf8_string);
    free(ustr->u8str);
}

void free_utf8(utf8_t *ustr) {
    free_utf8rep(ustr);
    free(ustr);
}

uint64_t utf8_strlen(utf8_t *ustr) {
    return ustr->u8str->chars;
}

bool utf8_strcmp(utf8_t *ustr1, utf8_t *ustr2) {
    if (ustr1->u8str->bytes == ustr2->u8str->bytes) {
        return false;
    } else {
        for (uint64_t i = 0; i < ustr1->u8str->bytes; i++) {
            if (ustr1->u8str->utf8_string[i] != ustr2->u8str->utf8_string[i]) {
                return false;
            }
        }
    }

    return true;
}


utf8_t *utf8_concat(utf8_t *ustr1, utf8_t *ustr2) {
    utf8_t *n_ustr = init_utf8();
    uint8_t *utf8_string = malloc(sizeof(uint8_t) *
                                 (ustr1->u8str->bytes +
                                  ustr2->u8str->bytes +
                                  DEFAULT_UTF8_STRING_LENGTH));
    memcpy(utf8_string,
           ustr1->u8str->utf8_string,
           sizeof(uint8_t) * ustr1->u8str->bytes);

    memcpy(utf8_string + ustr1->u8str->bytes,
           ustr2->u8str->utf8_string,
           sizeof(uint8_t) * ustr1->u8str->bytes);
    
    n_ustr->u8str->bytes = ustr1->u8str->bytes + ustr2->u8str->bytes;
    n_ustr->u8str->chars = ustr1->u8str->chars + ustr2->u8str->chars;
    n_ustr->u8str->length = ustr1->u8str->bytes + ustr2->u8str->bytes + DEFAULT_UTF8_STRING_LENGTH;

    if (ustr1->u8str->chars > ustr2->u8str->chars) {
        n_ustr->u8str->cache.inserted_length = ustr1->u8str->cache.inserted_length;
        n_ustr->u8str->cache.inserted_pos = ustr1->u8str->cache.inserted_pos;
    } else {
        n_ustr->u8str->cache.inserted_length = ustr2->u8str->cache.inserted_length;
        n_ustr->u8str->cache.inserted_pos = ustr2->u8str->cache.inserted_pos;
    }

    n_ustr->u8str->utf8_string = utf8_string;

    free_utf8(ustr1);
    free_utf8(ustr2);

    return n_ustr;
}