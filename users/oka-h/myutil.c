#include <limits.h>
#include "myutil.h"

void send_integer(uint32_t integer) {
    int result[16];
    int count = 0;

    if (integer == 0) {
        send_one_int(0);
        return;
    }

    while (integer > 0) {
        result[count++] = integer % 10;
        integer /= 10;
    }

    for (int i=count-1; i>=0; --i) {
        send_one_int(result[i]);
    }
}

void send_one_int(int number) {
    switch (number) {
        case 0:
            SEND_STRING("0");
            break;
        case 1:
            SEND_STRING("1");
            break;
        case 2:
            SEND_STRING("2");
            break;
        case 3:
            SEND_STRING("3");
            break;
        case 4:
            SEND_STRING("4");
            break;
        case 5:
            SEND_STRING("5");
            break;
        case 6:
            SEND_STRING("6");
            break;
        case 7:
            SEND_STRING("7");
            break;
        case 8:
            SEND_STRING("8");
            break;
        case 9:
            SEND_STRING("9");
            break;
    }
}
