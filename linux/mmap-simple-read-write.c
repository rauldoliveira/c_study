#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define DEFSZ 0x10000

static void *glob_var;

int main(void)
{
    int i;
    char *c = (char *) malloc(sizeof(char) * DEFSZ);

    glob_var = mmap(NULL, DEFSZ, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    /* fork and write string into mmaped area */
    memset(glob_var, '0', DEFSZ);
    if (fork() == 0) {
        sprintf((char *) glob_var, "%s", "test");
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
        sscanf(glob_var, "%s", c);
        printf("%s\n", c);
    }
    
    /* fork and write integer into mmaped area */
    memset(glob_var, '0', DEFSZ);
    if (fork() == 0) {
        sprintf((char *) glob_var, "%i", 666);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
        sscanf(glob_var, "%i", &i);
        printf("%i\n", i);
    }

    munmap(glob_var, DEFSZ);
    return 0;
}