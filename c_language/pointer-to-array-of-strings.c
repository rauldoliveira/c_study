/* This example used in the following scenario:
 * 1. there are several sets of files.
 * 2. we need to choose specific set.
 * 3. handle files from this set in the loop.
 */

#include<stdio.h>

int main()
{
int i;
char *set_a[4] = { "C", "C++", "Java", "VBA" };
char *set_b[4] = { "Tea", "Cofee", "Water", "Oil" };
char *set_c[4] = { "Earth", "Venus", "Mercury", "Mars" };
char *(*ptr)[4]; // = &arr;

/* here is the logic for choosing set */
ptr = &set_b;
  
for(i = 0; i < 4; i++)
    printf("String %d : %s\n",i+1,(*ptr)[i]);
return 0;
}