/*
 * use -lpthread when build.
 */

#include <semaphore.h>
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
    int i = 0;
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ |PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    glob_var = mmap(NULL, DEFSZ, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_init(sem, 1, 1);

    memset(glob_var, '0', DEFSZ);
    if (fork() == 0) {          /* child */
        while (1) {
            sem_wait(sem);
            sprintf((char *) glob_var, "%i", i);
            sem_post(sem);
            sleep(1);
            i++;
        }
        exit(EXIT_SUCCESS);
    }

    while (1) {                 /* parent */
        sscanf(glob_var, "%i", &i);
        printf("%i\n", i);
        sleep(1);
    }
    
    sem_destroy(sem);
    munmap(sem, sizeof(sem_t));
    munmap(glob_var, DEFSZ);
    return 0;
}