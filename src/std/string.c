#include "string.h"

u16 strlen(const char* s) {
    u16 len = 0;
    while (*(s + (int)len) != 0) {
        len += 1;
    }
    return len;
}

/*
 *  K&R (Kernighan & Ritchie) implementation
 */

void itoa(i16 n, char* s) {
    i16 i, sign;

    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void uitoa(u16 n, char* s) {
    u16 i;

    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    s[i] = '\0';

    reverse(s);
}

/*
 *  K&R (Kernighan & Ritchie) implementation
 */

void reverse(char* s) {
    i16 i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
