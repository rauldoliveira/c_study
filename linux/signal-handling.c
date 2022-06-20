#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("received SIGINT\n");
        exit(0);
    }
}

int main(void)
{
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    while(1) 
        sleep(1);
    return 0;
}