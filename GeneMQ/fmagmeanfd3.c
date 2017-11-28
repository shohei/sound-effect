#include <stdio.h>
#include <math.h>
/* #include "sinued.41.h" */
/* #include "fmagmeanfd3.h"*/
#include "sinupktNew.h"
#include "sinupktfrtr.h"
/*  -------------------------------------------------------------------
  fmagmeanfd3.c  < fmagmeanfd2 < fmagmeanfd1
   Description:
      meand of ins. frequency and squre mean of magnitude derivation
   input:  t,ntrall,ntr,index
   output: fmean,magmean,fstframe,lstframe
                     coded by Naotoshi osaka 6/23/1994

  index  0:  frtr[] known
         1:  frtr[] unknown
    revision history
    rev 1: 4/14/1995  fstframemin, lstframemax added
    ------------------------------------------------------------------ */
void  fmagmeanfd3(struct trackpkt tdum,struct frtrpkt frtr[],int ntrall,
        double fmean[],double magmean[],int fstframe[],int lstframe[],int ntr[],
        int *fstframemin,int *lstframemax,int n_int,int npk,int iord[],
        FILE *input,short irdie[],int index)

{ int i,ii,j,iifst,iilst,istart; double freq0,mag0,PI2;
extern void get_t(int j,int n_int,int npk,int iord[],struct trackpkt *tdum,
       FILE *input);

  PI2= 3.14159265358979 *2.0;
  /*  printf("fmagmeanfd3: ntrall=%d\n",ntrall); */

if(index!=0){   /* frtr[] unknown */
  for(i=ii=istart=0;i<ntrall;i++){
    if(i !=0){istart +=ntr[i-1];}
    fmean[i]=magmean[i]=0.0;
    fstframe[i] = 300000;
    lstframe[i] =  -1;
/*     printf("i=%d fstframe=%d lstframe=%d\n",i,fstframe[i],lstframe[i]);   */
    for(j=0;j<ntr[i];j++){           /* loop for one track */
    get_t(ii,n_int,npk,iord,&tdum,input);
      frtr[ii].iframe=tdum.iframe;
      frtr[ii].jtrack=tdum.jtrack;
      irdie[ii]=tdum.iroute;
/*    printf(" frtr[%d].iframe=%d frtr[%d]=jtrack=%d irdie[%d].iroute=%d\n",
      ii,frtr[ii].iframe,ii,frtr[ii].jtrack,ii,irdie[ii]); */
      if(frtr[ii].iframe<fstframe[i]){fstframe[i]=frtr[ii].iframe;}
      if(frtr[ii].iframe>lstframe[i]){lstframe[i]=frtr[ii].iframe;}
/*     printf("inside if i=%d fstframe=%d lstframe=%d\n",
         i,fstframe[i],lstframe[i]); */
      fmean[i] += (freq0=(double)tdum.nOmega0/pow(2.,8.)/PI2);
      magmean[i] += (mag0=(double)tdum.Mag0*(double)tdum.Mag0);
      
/*  printf("ii=%4d tr%3d fr%3d F= %7.1lf[Hz] Mag=%7.1lf nTheta0=%lf Theta0=%lf    Theta1=%lf nThata1=%lf\n",
      ii,t[ii].jtrack,t[ii].iframe,freq0,mag0,t[ii].nTheta0,t[ii].Theta0,
      t[ii].nTheta1,t[ii].Theta1);   */

/*    printf("process(%2d):fr=%d fstframe[%d]=%d lstframe[%d]=%d ntr[%d]=%d\n",
      j,t[ii].iframe,i,fstframe[i],i,lstframe[i],i,ntr[i]);  */

     ii++;
  } /* end of j */
    fmean[i] /=(double)ntr[i];
    magmean[i] /= (double)ntr[i];
    magmean[i]= sqrt(magmean[i]);
/*    printf("fmagmeanfd3: ntr[%d]=%d fstframe[%d]=%d lstframe[%d]=%d fmean[%d]=%lf magmean[%d]=%lf\n",
      i,ntr[i],i,fstframe[i],i,lstframe[i],i,fmean[i],i,magmean[i]);  */
} /* end of i */
} /* end of if index !=0 */
else {  /* index==0; fstframe known; */

  for(i=ii=istart=0;i<ntrall;i++){
    if(i !=0){istart +=ntr[i-1];}
    fmean[i]=magmean[i]=0.0;
    fstframe[i] = frtr[(iifst=istart)].iframe;
    lstframe[i] = frtr[(iilst=istart+ntr[i]-1)].iframe;
/*     printf("i=%d fstframe=%d lstframe=%d\n",i,fstframe[i],lstframe[i]);   */
    for(j=0;j<ntr[i];j++){           /* loop for one track */
    get_t(ii,n_int,npk,iord,&tdum,input);
/*      printf(" frtr[%d].iframe=%d\n",ii,frtr[ii].iframe); */
      if(frtr[ii].iframe<fstframe[i]){fstframe[i]=frtr[ii].iframe; iifst=ii;}
      if(frtr[ii].iframe>lstframe[i]){lstframe[i]=frtr[ii].iframe; iilst=ii;}
/*     printf("inside if i=%d fstframe=%d lstframe=%d\n",
             i,fstframe[i],lstframe[i]); */
      fmean[i] += (freq0=(double)tdum.nOmega0/pow(2.,8.)/PI2);
      magmean[i] += (mag0=(double)tdum.Mag0*(double)tdum.Mag0);
      
/*  printf("ii=%4d tr%3d fr%3d F= %7.1lf[Hz] Mag=%7.1lf nTheta0=%lf Theta0=%lf    Theta1=%lf nThata1=%lf\n",
      ii,t[ii].jtrack,t[ii].iframe,freq0,mag0,t[ii].nTheta0,t[ii].Theta0,
      t[ii].nTheta1,t[ii].Theta1);   */

/*    printf("process(%2d):fr=%d fstframe[%d]=%d lstframe[%d]=%d ntr[%d]=%d\n",
      j,t[ii].iframe,i,fstframe[i],i,lstframe[i],i,ntr[i]);  */

     ii++;
  } /* end of j */
    fmean[i] /=(double)ntr[i];
    magmean[i] /= (double)ntr[i];
    magmean[i]= sqrt(magmean[i]);
/*    printf("fmagmeanfd3: ntr[%d]=%d fstframe[%d]=%d lstframe[%d]=%d fmean[%d]=%lf magmean[%d]=%lf\n",
      i,ntr[i],i,fstframe[i],i,lstframe[i],i,fmean[i],i,magmean[i]);  */
} /* end of i */

} /* end of big else */

    (*fstframemin)= fstframe[0];
    for(i= (*lstframemax)=0;i<ntrall;i++){ /* rev. 1 */
        if(fstframe[i]<*fstframemin){*fstframemin=fstframe[i];}
        if(lstframe[i]>*lstframemax){*lstframemax=lstframe[i];}
    }
  for(i=0;i<ntrall;i++){
/*    printf(" final: fstframe[%d]=%d lstframe[%d]=%d ntr[%d]=%d\n", 
      i,fstframe[i],i,lstframe[i],i,ntr[i]); */
  }  
/*    printf(" fstframemin=%d lstframemax=%d\n", *fstframemin, *lstframemax); */
}
