#include "utf8.h"

uchar_t utf8_nul = {
    .is_error = false,
    .length = 1,
    .uchar = {0, 0, 0, '\0'}};

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

uint8_t get_byte_length(uint8_t *bytes, uint8_t max_length) {
    if (is_1byte(bytes[0])) {
        return 1;
    } else if (is_2byte(bytes[0]      &&
               max_length >= 2        &&
               is_trailing(bytes[1]))) {
        return 2;
    } else if (is_3byte(bytes[0]      &&
               max_length >= 3        &&
               is_trailing(bytes[1])  &&
               is_trailing(bytes[2]))) {
        return 3;
    } else if (is_4byte(bytes[0]      &&
               max_length >= 4        &&
               is_trailing(bytes[1])  &&
               is_trailing(bytes[2])  &&
               is_trailing(bytes[3]))) {
        return 4;
    } else {
        return 0;
    }
                   
}

utf8_t *init_utf8() {
    utf8_t *ustr = malloc(sizeof(utf8_t));
    utf8_cache_t ustr_cache;

    ustr->is_error = false;

    ustr_cache.inserted_length = 0;
    ustr_cache.inserted_pos = 0;

    ustr->utf8.str = malloc(sizeof(uint8_t) * MAXIMUM_UTF8_STRING_LENGTH);
    ustr->utf8.length = MAXIMUM_UTF8_STRING_LENGTH;
    ustr->utf8.bytes = 0;
    ustr->utf8.chars = 0;
    ustr->utf8.cache.inserted_length = 0;
    ustr->utf8.cache.inserted_pos = 0;

    return ustr;
}

void utf8_append_char(utf8_t *ustr, uchar_t uc) {

    if (ustr->is_error || uc.is_error) {
        return;
    } else if ((ustr->utf8.length - ustr->utf8.bytes) <= MAXIMUM_UTF8_STRING_LENGTH) {
        grow_string(ustr);
    }

    uint64_t ubytes = ustr->utf8.bytes;

    switch (uc.length) {
        case 1:
            ustr->utf8.str[ubytes] = uc.uchar[0];
            ustr->utf8.bytes += uc.length;
            break;
        case 2:
            ustr->utf8.str[ubytes] = uc.uchar[1];
            ustr->utf8.str[ubytes + 1] = uc.uchar[0];
            ustr->utf8.bytes += uc.length;
            break;
        case 3:
            ustr->utf8.str[ubytes] = uc.uchar[2];
            ustr->utf8.str[ubytes + 1] = uc.uchar[1];
            ustr->utf8.str[ubytes + 2] = uc.uchar[0];
            ustr->utf8.bytes += uc.length;
            break;
        case 4:
            ustr->utf8.str[ubytes] = uc.uchar[3];
            ustr->utf8.str[ubytes + 1] = uc.uchar[2];
            ustr->utf8.str[ubytes + 2] = uc.uchar[1];
            ustr->utf8.str[ubytes + 3] = uc.uchar[0];
            ustr->utf8.bytes += uc.length;
            break;
        default:
            ustr->is_error = true;
            ustr->error.err_code = INVALID_UTF8;
            return;
    }

    ustr->utf8.chars++;
}

uchar_t get_char(uint8_t *bytes, uint8_t max_length) {
    uchar_t c = {
        .is_error = false,
        .length = 0,
        .uchar = {0, 0, 0, 0}};

    switch (get_byte_length(bytes, max_length)) {
        case 1:
            memcpy(c.uchar,
                  (uint8_t[4]){bytes[0], 0, 0, 0},
                  sizeof(uint8_t) * 4);
            c.length = 1;
            break;
        case 2:
            memcpy(c.uchar,
                  (uint8_t[4]){bytes[0], bytes[1], 0, 0},
                  sizeof(uint8_t) * 4);
            c.length = 2;
            break;
        case 3:
            memcpy(c.uchar,
                  (uint8_t[4]){bytes[1], bytes[2], bytes[3], 0},
                  sizeof(uint8_t) * 4);
            c.length = 3;
            break;
        case 4:
            memcpy(c.uchar,
                  (uint8_t[4]){bytes[0], bytes[1], bytes[2], bytes[3]},
                  sizeof(uint8_t) * 4);
            c.length = 4;
            break;
        default:
            c.is_error = true;
            c.error.err_code = INVALID_UTF8;
            return c;
    }

    return c;
}

utf8_t *convert_to_utf8(uint8_t *bytes, uint64_t len) {
    utf8_t *ustr = init_utf8();

    if (bytes == NULL) {
        ustr->is_error = true;
        ustr->error.err_code = NULL_ERROR;
        return ustr;
    }

    uint64_t cnt = 0;
    uchar_t c;

    while (bytes[cnt] != '\0' && (cnt < len)) {
        c = get_char(bytes + cnt, min(4, len - cnt));

        if (c.is_error) {
            ustr->is_error = true;
            ustr->error.err_code = INVALID_UTF8;
            return ustr;
        } else {
            utf8_append_char(ustr, c);
            cnt += c.length;
        }
    }

    utf8_append_char(ustr, utf8_nul);

    return ustr;
}

void grow_string(utf8_t *ustr) {
    // double the possible string length
    utf8str_t utf8 = {
        .str = malloc(sizeof(uint8_t) * ustr->utf8.length * 2),
        .length = ustr->utf8.length * 2,
        .bytes = ustr->utf8.bytes,
        .chars = ustr->utf8.chars,
        .cache.inserted_length = ustr->utf8.cache.inserted_length,
        .cache.inserted_pos = ustr->utf8.cache.inserted_pos,
    };    

    memcpy(utf8.str, ustr->utf8.str, sizeof(uint8_t) * ustr->utf8.bytes);

    free_utf8str(ustr);

    ustr->utf8 = utf8;
}

void strip_last_byte(utf8_t *ustr) {
    if (ustr->utf8.bytes > 0 &&
        ustr->utf8.str[ustr->utf8.bytes] == '\0') {
        ustr->utf8.bytes--;
        ustr->utf8.chars--;
    }
}

void free_utf8str(utf8_t *ustr) {
    free(ustr->utf8.str);
}

void free_utf8(utf8_t *ustr) {
    free_utf8str(ustr);
    free(ustr);
}

uint64_t utf8_strlen(utf8_t *ustr) {
    return ustr->utf8.chars;
}

utf8_t *utf8_concat(utf8_t *ustr1, utf8_t *ustr2) {
    strip_last_byte(ustr1);

    utf8_t *n_ustr = init_utf8();
    uint8_t *str = malloc(sizeof(uint8_t) *
                                 (ustr1->utf8.bytes +
                                  ustr2->utf8.bytes +
                                  MAXIMUM_UTF8_STRING_LENGTH));

    memcpy(str,
           ustr1->utf8.str,
           sizeof(uint8_t) * ustr1->utf8.bytes);

    memcpy(str + ustr1->utf8.bytes,
           ustr2->utf8.str,
           sizeof(uint8_t) * ustr2->utf8.bytes);
    
    n_ustr->utf8.bytes = ustr1->utf8.bytes + ustr2->utf8.bytes;
    n_ustr->utf8.chars = ustr1->utf8.chars + ustr2->utf8.chars;
    n_ustr->utf8.length = ustr1->utf8.bytes + ustr2->utf8.bytes + MAXIMUM_UTF8_STRING_LENGTH;

    if (ustr1->utf8.chars > ustr2->utf8.chars) {
        n_ustr->utf8.cache.inserted_length = ustr1->utf8.cache.inserted_length;
        n_ustr->utf8.cache.inserted_pos = ustr1->utf8.cache.inserted_pos;
    } else {
        n_ustr->utf8.cache.inserted_length = ustr2->utf8.cache.inserted_length;
        n_ustr->utf8.cache.inserted_pos = ustr2->utf8.cache.inserted_pos;
    }

    n_ustr->utf8.str = str;

    return n_ustr;
}

uchar_t utf8_access(utf8_t *ustr, uint64_t pos) {

    uchar_t c = {.is_error = false, .length = 0, .uchar = {0, 0, 0, 0}};
    uint8_t *str = ustr->utf8.str;

    if (pos > ustr->utf8.chars) {
        ustr->is_error = true;
        ustr->error.err_code = OUT_OF_BOUND_ERROR;

        return c;
    }

    for (uint64_t i = 0, byte = 0, uc_len = 0; i < pos; i++, byte += uc_len) {
        if (i == pos) {
            c = get_char(str + byte, min(4, ustr->utf8.bytes - byte));
            return c;   
        } else {
            uc_len = get_byte_length(str, min(4, ustr->utf8.bytes - byte));
        }

        if (uc_len == 0) {
                c.is_error = true;
                c.error.err_code = INVALID_UTF8;
        }
    }

    return c;
}