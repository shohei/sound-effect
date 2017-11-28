#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include "sinupktNew.h"
#include "sinupktfrtr.h"
#define MAXARG 10
#define MAXCOLUMN 100

/*  ---------------------------------------------------------------
     Synthesis/Modification operation using sinusoidal model's packet
     systemcall:  systemcall functions:
     This is for UNIX.
     Note: Different version for Windows should be used if called under
           Windows.
     coded by Naotoshi Osaka    May 20, 2000
    -------------------------------------------------------------- */
int systemcall(char *fntmp0,char *fntmp1,char *fntmp,
               char *fdir_template,char *fn_input,char *filepkt,
               char *filepktout,FILE *input)
{ int ie;
  strcat(strcpy(fntmp0,"rm "),strcat(strcat(strcpy(fntmp,fdir_template),fn_input),filepkt));
  printf("fntmp0=%s\n",fntmp0); /* rm [filename] */
  fclose(input);  /* input packet file */
  ie=system(fntmp0);
  printf(" pass 02 ie=%d\n",ie);  
  strcpy(fntmp0,"mv ");
  strcat(strcat(strcpy(fntmp1,fdir_template),fn_input),filepktout);
  strcat(fntmp0,strcat(strcat(fntmp1," "),fntmp));
      /* mv [filename].pkt.bf [filename].pkt  */
  printf("fntmp0=%s\n",fntmp0);
  ie=system(fntmp0);
 printf(" pass 03 id=%d fntmp0=%s \n",ie,fntmp0);  
 return(0);
}
