/* -----------------------------------------------------------------------
    get_cmd.c
     get command from terminal after prompt.
          coded by Naotoshi Osaka   4/28/'00
   ----------------------------------------------------------------------- */
#include <string.h> 
#include <math.h>
#include <stdio.h>

void get_cmd(char prompt[],char cmdline[],int maxline)
{ int getaline(char s[],int lim);
  int i;

 printf(prompt);
  for(i=0;i<maxline;i++){cmdline[i]=' ';} /* null */
  getaline(cmdline,maxline);
}
/* get a line from keyboard -- page 31 in textbook */
int getaline(char s[],int lim)
{int c, i;
 for(i=0;i<lim-1 && (c=getchar())!=EOF && c!='\n'; ++i) s[i]=c;
 s[i]='\0'; return(i);
 }
