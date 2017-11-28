/* -----------------------------------------------------------------------
    commandanal2.c
     command analysis for sinued.c
    ver.1 : from commandanal.c
          coded by Naotoshi Osaka   4/30/'00
   ----------------------------------------------------------------------- */
#include <string.h> 
#include <math.h>
#include <stdio.h>
#define MAXARG 10
int commandanal2(char cmdline[],char *cmnd[],int ncomd,char ***arg,
    int *icomd)
{ int i,ifound,icnum;
  char nest[100]; int narg;


  for(i=0;i<MAXARG;i++){(*arg)[i][0]='\0';}
   sscanf(cmdline,"%s %s %s %s %s %s %s %s %s %s",(*arg)[0],(*arg)[1],
               (*arg)[2],(*arg)[3],(*arg)[4],(*arg)[5],(*arg)[6],
               (*arg)[7],(*arg)[8],(*arg)[9]);
   for(i=narg=0;i<MAXARG;i++){
     if(strlen((*arg)[i])==0){break;}
     narg++;
    }
  if(narg==0)return(-1);
  for(i=ifound=0;i<ncomd;i++){
    if(strcmp((*arg)[0],cmnd[i])==0){icnum=i; ifound=1; break;}
  }
  if(ifound==0){printf("command error\n"); return(-1);}
  else if (icnum==0){return(*icomd=icnum);}         /* --- ex command ---- */

  *icomd=icnum;
  return(narg);
}
