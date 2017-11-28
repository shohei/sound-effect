#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
/* #include "sinupktNew.h"
#include "sinupktfrtr.h" */
#define MAXARG 10
#define MAXCOLUMN 100
/*  ---------------------------------------------------------------
     Synthesis/Modification operation using sinusoidal model's packet
     zed: to discard zero magnitude packets
     coded by Naotoshi Osaka    May 20, 2000
    -------------------------------------------------------------- */
int cmd_zed(int *ntrallcommon,int n_int,int *fstframemin,int *lstframemax,
           FILE *input)
{ int i,k,ii,jj;
  void init_trkdt(int *ntrallcommon,int *fstframemin,
       int *lstframemax,int n_int,FILE *input);
  extern void get_t(int j,int n_int,int npk,int iord[],
       struct trackpkt *tdum,FILE *input);

if(trackorder !=0){ /* frame order */
  printf(" Do cv command first\n");
  return(0);
}
else { /* trackorder */
    jj= k= -1;
    for(i=ii=ntrall=0;i<ndata;i++){
   get_t(i,n_int,npk,iord,&tdum,input);
      if((double)tdum.Mag0!=0 || (double)tdum.Mag1!=0){
         if(tdum.jtrack !=jj){k++;ntrall++;jj=tdum.jtrack;ntr[k]=1;}
         else {ntr[k]++;}
         frtr[ii].iframe=frtr[i].iframe;
         frtr[ii].jtrack=frtr[i].jtrack;
         irdie[ii]= irdie[i];
         iord[ii++]=iord[i];
     }
  }
   ndata= ii;
}
 printf(" ndata=%d ntrall=%d \n",ndata,ntrall);
init_trkdt(ntrallcommon,fstframemin,lstframemax,n_int,input); /* OP: trkdata init */
return(0);
}

