/*
 * Special thanks to chqrlie for
 * http://stackoverflow.com/questions/38940390/c-sorting-3d-string-array-by-specific-column
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static char * rand_string(size_t ssize)
{
    char * str = malloc(sizeof(char) * ssize);
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    if (ssize) {
        --ssize;
        for (size_t n = 0; n < ssize; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[ssize] = '\0';
    }
    return str;
}

char *** init_array(char *** array, int n_rows, int n_cols, int ssize)
{
    int i, j;
    array = malloc(n_rows * sizeof(char **));
    for (i = 0; i < n_rows; i++)
    {
        array[i] = malloc(n_cols * sizeof(char *));
        for (j = 0; j < n_cols; j++)
            array[i][j] = malloc(ssize * sizeof(char));
    }
    return array;
}

void gen_array(char *** array, int n_rows, int n_cols, int ssize)
{
    int i, j, k;
    for (i = 0; i < n_rows; i++)
        for (j = 0; j < n_cols; j++)
            snprintf(array[i][j], ssize, "%s", rand_string(ssize));
}

int str_compare(const void * a, const void * b)
{
    const char *pa = ((const char ***) a)[0][2];
    const char *pb = ((const char ***) b)[0][2];

    return strcmp(pa, pb);
}

void print_array(char *** array, int n_rows, int n_cols)
{
    int i, j;
    for (i = 0; i < n_rows; i++) {
        for (j = 0; j < n_cols; j++)
            printf("%s ", array[i][j]);
        printf("\n");
    }
}

int main(void)
{
    int n_rows = 3, n_cols = 5, ssize = 10;
    char *** z;

    z = init_array(z, n_rows, n_cols, ssize);
    gen_array(z, n_rows, n_cols, ssize);
    print_array(z, n_rows, n_cols);
    printf("\n");
    qsort(z, 3, sizeof(char **), str_compare);
    print_array(z, n_rows, n_cols);

    return 0;
}