#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n_rows = 10, n_cols = 20, tup_size=64;
    char ***z;

    z = malloc(n_rows * sizeof(char **));
    assert(z != NULL);
    for (i = 0; i < n_rows; ++i)
    {
        z[i] = malloc(n_cols * sizeof(char *));
        assert(z[i] != NULL);
        for (j = 0; j < n_cols; ++j)
        {
            z[i][j] = malloc(tup_size);
            assert(z[i][j] != NULL);
        }
    }
    return 0;
}