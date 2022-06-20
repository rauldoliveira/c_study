#include <stdio.h>                                                                                                                                                      
#include <string.h>                                                                                                                                                     
int main(void)                                                                                                                                                          
{                                                                                                                                                                       
    FILE *fp;
    fp = fopen("/proc/meminfo", "r");
    char buffer[121];
    char key[80];
    long int value;

    while (fgets(buffer, 120, fp) != NULL) {
        sscanf(buffer, "%s %li", &key, &value);
        if (!strcmp(key,"MemTotal:"))
            printf("%s %li\n", key, value);
        else if (!strcmp(key,"MemFree:"))
            printf("%s %li\n", key, value);
        else if (!strcmp(key,"Buffers:"))
            printf("%s %li\n", key, value);
        else if (!strcmp(key,"Cached:"))
            printf("%s %li\n", key, value);
        else if (!strcmp(key,"SwapTotal:"))
            printf("%s %li\n", key, value);
        else if (!strcmp(key,"SwapFree:"))
            printf("%s %li\n", key, value);
        else if (!strcmp(key,"Dirty:"))
            printf("%s %li\n", key, value);
        else if (!strcmp(key,"Writeback:"))
            printf("%s %li\n", key, value);
    }
    fclose(fp);
    return 0;
}