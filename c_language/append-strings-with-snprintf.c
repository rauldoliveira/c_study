#include <stdio.h>
#include <string.h>

#define LOC_MAXLEN 13

int main (void)
{
  char dest[LOC_MAXLEN];
  snprintf(dest, LOC_MAXLEN, "%s%s", "abc", "def");

  printf("%s\n", dest);
  
  /* append new string using length of previously added string */
  snprintf(dest + strlen(dest), LOC_MAXLEN - strlen(dest), "%s", "ghi");
  printf("%s\n", dest);

  /* repeat that */
  snprintf(dest + strlen(dest), LOC_MAXLEN - strlen(dest), "%s", "jkl");
  printf("%s\n", dest);
  
  return 0;
}