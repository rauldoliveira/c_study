#include <stdio.h>
#include <stdlib.h>
void doit(int ** s , int row, int col)
{
    int i, j;
    for (i = 0; i < row; i++){
        for (j = 0 ;j < col; j++)
            printf("%d ", s[i][j]);
        printf("\n");
    }
}
int main()
{
    int i, j;
    int row = 4, col = 10;
    int **c = (int**) malloc(sizeof(int*) * row);
    for (i = 0; i < row; i++)
        *(c+i) = (int*) malloc(sizeof(int) * col);
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            c[i][j] = i * j;
    doit(c, row, col);
}