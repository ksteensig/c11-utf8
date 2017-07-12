#include "utf8.h"

#include <string.h>
#include <stdio.h>

int main() {
    uint8_t *str = (uint8_t *)"a utf8 string";
    uint8_t *str2 = (uint8_t *)"Ḽơᶉëᶆ ȋṕšᶙṁ ḍỡḽǭᵳ ʂǐť ӓṁệẗ, ĉṓɲṩḙċťᶒțûɾ ấɖḯƥĭṩčįɳġ ḝłįʈ, șếᶑ ᶁⱺ ẽḭŭŝḿꝋď ṫĕᶆᶈṓɍ ỉñḉīḑȋᵭṵńť ṷŧ ḹẩḇőꝛế éȶ đꝍꞎôꝛȇ ᵯáꞡᶇā ąⱡîɋṹẵ.";

    utf8_t *ustr = convert_to_utf8(str, strlen((char *)str) + 1);
    utf8_t *ustr2 = convert_to_utf8(str2, strlen((char *)str2) + 1);
    utf8_t *ustr3 = utf8_concat(ustr, ustr2);

    printf(u8"%s\n", ustr->utf8.str);
    printf(u8"%s\n", ustr2->utf8.str);
    printf(u8"%s\n", ustr3->utf8.str);

    return 0;
}
