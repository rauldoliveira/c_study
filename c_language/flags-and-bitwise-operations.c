#include <stdio.h>
#include <string.h>

#define DO_ACTIVE 1 << 0
#define DO_IDLE 1 << 1
#define DO_IDLE_IN_XACT 1 << 2
#define DO_WAITING 1 << 3
#define DO_OTHER 1 << 4

int main(void)
{
    char input[5] = "xw";
    int options = 0, i;

    if (strlen(input) > 5) {
        printf("mask too long\n");
        return 0;
    }

    for (i = 0; i < strlen(input); i++) {
        switch (input[i]) {
            case 'a':
                options |= DO_ACTIVE;
                break;
            case 'i':
                options |= DO_IDLE;
                break;
            case 'x':
                options |= DO_IDLE_IN_XACT;
                break;
            case 'w':
                options |= DO_WAITING;
                break;
            case 'o':
                options |= DO_OTHER;
                break;
        }
    }

    if (options == 0)
        printf("empty\n");
    if (options & DO_ACTIVE)
        printf("active\n");
    if (options & DO_IDLE)
        printf("idle\n");
    if (options & DO_IDLE_IN_XACT)
        printf("idle in xact\n");
    if (options & DO_WAITING)
        printf("waiting\n");
    if (options & DO_OTHER)
        printf("other\n");
    return 0;
}