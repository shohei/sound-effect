#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "sinupktNew.h"
#include "sinupktfrtr.h"
/* ----------------------------------------------------------------------------
   get packet from file sequencially
   coded by Naotoshi Osaka  4.20.1999
   ----------------------------------------------------------------------------- */
void get_t(int j,int n_int,int npk,int iord[],struct trackpkt *tdum,FILE *input)
{ int ii,ie;

/*  if(j>=30000){  printf("j=%d iord[%d]=%d \n",j,j,iord[j]); } */
    if(iord[j]!= -1){

      if(j==0){ /* initiallize */
        ii=(sizeof(int)*n_int + sizeof(short)*npk*(int)iord[j]);
/*       printf("1st position=%d n_int=%d npk=%d iord[j]=%d\n",
         ii,n_int,npk,iord[j]); */
      fseek(input,(long)(ii=(sizeof(int)*n_int + sizeof(short)*npk*(int)iord[j])),
            SEEK_SET);
  }
      else {
       /*  if(j>=30000){  ii=sizeof(short)*npk*(int)(iord[j]-iord[j-1]-1);
       printf("2ndafter  postion=%d\n",ii);
       }  */
      fseek(input,(long)(sizeof(short)*npk*(int)(iord[j]-iord[j-1]-1)),SEEK_CUR);
     }
     /* ---  Read data --- */
       ie=fread(tdum,sizeof(short),npk,input);
     /* if(j>=30000){ printf("j=%d tdum.iframe=%d tdum.jtrack=%d tdum.iroute=%d\n",
        j,tdum->iframe,tdum->jtrack,tdum->iroute);
      } */


    }  /* end of if (iord[j]!= -1) */

}
/* ----------------------------------------------------------------------------
   get packet from file randomly
   coded by Naotoshi Osaka  5.25.1999
   ----------------------------------------------------------------------------- */
void get_trand(int j,int jold,int n_int,int npk,int iord[],struct trackpkt *tdum,
FILE *input)
{ int ii,ie;

/*  if(j>=30000){  printf("j=%d iord[%d]=%d \n",j,j,iord[j]); } */
    if(iord[j]!= -1){

      if(j==0){ /* initiallize */
        ii=(sizeof(int)*n_int + sizeof(short)*npk*(int)iord[j]);
/*       printf("1st position=%d n_int=%d npk=%d iord[j]=%d\n",
         ii,n_int,npk,iord[j]); */
      fseek(input,(long)(ii=(sizeof(int)*n_int + sizeof(short)*npk*(int)iord[j])),
            SEEK_SET);
  }
      else {
       /*  if(j>=30000){  ii=sizeof(short)*npk*(int)(iord[j]-(iord[jold]+1));
       printf("2ndafter  postion=%d\n",ii);
       }  */
      fseek(input,(long)(sizeof(short)*npk*(int)(iord[j]-(iord[jold]+1))),SEEK_CUR);
     }
     /* ---  Read data --- */
       ie=fread(tdum,sizeof(short),npk,input);
     /* if(j>=30000){ printf("j=%d tdum.iframe=%d tdum.jtrack=%d tdum.iroute=%d\n",
        j,tdum->iframe,tdum->jtrack,tdum->iroute);
      } */


    }  /* end of if (iord[j]!= -1) */

}
/* ----------------------------------------------------
   sysparsetCom
  Description: input system parameter in common from disk file
    coded by Naotoshi Osaka 2/16/1994
   ---------------------------------------------------- */
int sysparsetCom(FILE *input,int *nframemax,int *ntrackmax,int *nframe,int *ntrall,
             int *npk,double *sf,int *smsec,int *nfft,int *nwdw,int *trackorder,
             int *ndata,double *threshdb,int *threshlength,int **nharm,int **ntr,
             double **trkpwr,int **jdgtr,int idw)

{  extern void *alocbuf(int unit,int nsize) ;
   int ie,i,j,idum,npk0,n_int;
    ie=fread(nframemax,sizeof(int),1,input);
    ie=fread(ntrackmax,sizeof(int),1,input);
    ie=fread(nframe,sizeof(int),1,input);
    ie=fread(ntrall,sizeof(int),1,input);
    ie=fread(npk,sizeof(int),1,input);
    ie=fread(sf,sizeof(double),1,input);
    ie=fread(smsec,sizeof(int),1,input); 
    ie=fread(nfft,sizeof(int),1,input);
    ie=fread(nwdw,sizeof(int),1,input);
    if(idw != -1){ ie=fread(trackorder,sizeof(int),1,input);}
    else {fread(&idum,sizeof(int),1,input);}
    ie=fread(ndata,sizeof(int),1,input);
    ie=fread(threshdb,sizeof(double),1,input);
    ie=fread(threshlength,sizeof(int),1,input);
    n_int=15;
 if(*trackorder==0){  /* track order synthesis */
    *ntr  =alocbuf(sizeof(int),*ntrall);
    *trkpwr =alocbuf(sizeof(double),*ntrall);
    *jdgtr  =alocbuf(sizeof(int),*ntrall);

    ie=fread(*ntr,sizeof(int),*ntrall,input);
    ie=fread(*trkpwr,sizeof(double),*ntrall,input);
    ie=fread(*jdgtr,sizeof(int),*ntrall,input);
    n_int += (*ntrall * 4);
 }
 else {   /* frame order synthesis */
    *nharm  =alocbuf(sizeof(int),*nframe);
    ie=fread(*nharm,sizeof(int),*nframe,input);
    n_int += *nframe;
    if(*npk<0){/* THE VERY FIRST MODIFICATION AFTER SINU. Data length is nframemax */
      /*  HERE *nframemax is NEWLY defined from sinued.43.c */
    printf(" First modification\n");
    fseek(input,(long)(sizeof(int)*(*nframemax-*nframe)),SEEK_CUR);
    *npk *= (-1);
    n_int += (*nframemax - *nframe);
    *nframemax= *nframe;
    }
}

/*    for(i=j=0;i<*ntrall;i++){
      if((*jdgtr)[i]==0){(*trkpwr)[j++]=(*trkpwr)[i];}
    }
    *ntrall=j; */

/* monitor */
 if(idw==1 || idw == -2){
   printf(" System variables \n");
   printf(" nframemax=%d\n ntrackmax=%d\n nframe=%d\n ntrall=%d\n npk=%d\n",
           *nframemax,*ntrackmax,*nframe,*ntrall,*npk);
   printf(" sf=%lf\n smsec=%d\n nfft=%d\n trackorder=%d\n ndata=%d\n",
           *sf,*smsec,*nfft,*trackorder,*ndata);
   printf(" threshdb=%lf\n threshlength=%d\n ",
    *threshdb,*threshlength);
/*   for(i=0;i<ntrall;i++){
     printf(" ntr[%d]=%d trkpwr[%d]=%lf jdgtr[%d]=%d\n",
     i,(*ntr)[i],i,(*trkpwr)[i],i,(*jdgtr)[i]); 
   } */
 }
return(n_int);
}
/* -----------------------------------------------------------------
   gensinu3
   Description:
     generate sound from sinusoidal model packet data
                   coded by Naotoshi Osaka 2/16/1994
                 modified by Naotoshi osaka 12/13/1998
   ----------------------------------------------------------------- */
void gensinu3(int iframe,int jtrack,double nTheta0,double nTheta1,
         double nOmega0,double nOmega1,double Mag0,double Mag1,int nwdw,
         int iroute,double rbuf[],double rpbuf[],
         int icf,int icm,int icd,int nshift,double sf,double PI2,double ssec)
{int n,index,jtrackp1,ie;
 double miu1,matrixup,matrixdown,Alpha,Beta,theta,foft,nbias,insferr;
 double t;
 double x,y,M;
 static double wavc[31][31];
extern double polyval(double p[],double x,int n);
extern int wavshpcoef(double wavc[31][31],int n,double a);
/* ---------------------- */
jtrackp1 = jtrack+1;
/* printf("gensinu3: iframe=%d jtrack+1=%d f0=%6.1lf f1=%6.1lf Mag0=%5.1lf Mag1\
=%5.1lf nTheta0=%5.2lf nTheta1=%5.2lf \n",
        iframe,jtrackp1,nOmega0/(2.*PAI),nOmega1/(2.*PAI),        Mag0,Mag1,nTheta0,nTheta1); */
 index=1; if(iroute==1 || iroute==3)index=0;
 if(icf ==1 || icf==2){
  ie=wavshpcoef(wavc,jtrackp1,1.0);
  printf("jtrackp1=%d 3rd: %5.1lf %5.1lf %5.1lf %5.1lf \n",
   jtrack,wavc[jtrackp1][0],wavc[jtrackp1][1],wavc[jtrackp1][2],
     wavc[jtrackp1][3]);
}  /* wavshaping coef. */

  if(index==1){
   for(n=0;n<nshift;n++){     /* ---- loop for each sample ---- */
        t=(double)n/(sf*1000.0);
    theta=nTheta0+nOmega0*t;
/*    if(n==0 || n== nshift-1){
    printf("1st order: theta=%lf \n",theta);
    printf(" n=%4d  nOmega0=%lf nOmega1=%lf nTheta0=%lf nTheta1=%lf\n",
               n,nOmega0,nOmega1,nTheta0,nTheta1); } */
    if(icf==1 || icf==2){   /* waveshaping */
      rbuf[n] = (Mag0+(Mag1-Mag0)*(double)n/(double)nshift)
              * polyval(wavc[jtrackp1],cos(fmod(theta,PI2)),jtrackp1);
      rpbuf[n] = theta*(double)jtrack;
    }
    else{
      rbuf[n] = (Mag0+(Mag1-Mag0)*(double)n/(double)nshift)
              * cos(fmod(theta,PI2));
      rpbuf[n] = theta;
    }  /* end of else */
    }                          /* ---- loop end for each sample --- */
    insferr=0.0;
}
  else{
   x= ((nTheta0 + nOmega0*ssec-nTheta1)
      +(nOmega1-nOmega0)*ssec/2.0)/(PI2);
   M= x-(M=(double)((int)x)) < M+1.0-x ? M :M+1.0 ;
   miu1=M-x;

   matrixup  =nTheta1-nTheta0-nOmega0*ssec+M*PI2;
   matrixdown=nOmega1-nOmega0;
   Alpha=matrixup*(3.0/(ssec*ssec))-matrixdown/ssec;
   Beta =matrixup*(-2.0/(ssec*ssec*ssec))+matrixdown/(ssec*ssec);
    insferr =0.;
    nbias=(nwdw/2*2==nwdw ? nwdw/2 : (nwdw-1)/2);
   for(n=0;n<nshift;n++){     /* ---- loop for each sample ---- */
        t=(double)n/(sf*1000.0);
    theta=nTheta0+nOmega0*t+Alpha*t*t+Beta*t*t*t;
    foft = nOmega0+2.0*Alpha*t+3.0*Beta*t*t;
/*    printf(" cubic: theta=%lf \n",theta);  */
    if(icf==1 || icf==2){   /* waveshaping */
      rbuf[n] = (Mag0+(Mag1-Mag0)*(double)n/(double)nshift)
               * (y=polyval(wavc[jtrackp1],(x=cos(fmod(theta,PI2))),jtrackp1));
/*      printf(" i=%d jtrack=%d x=%lf y=%lf\n",n,jtrack,x,y); */
      rpbuf[n]  = theta*(double)jtrack;
    }
    else {
      rbuf[n] = (Mag0+(Mag1-Mag0)*(double)n/(double)nshift)
               * cos(fmod(theta,PI2));
      rpbuf[n]  = theta;
    }    /* end of else above */
      /* insf evaluation */
/*      insferr +=
      (foft-insf[nbias+iframe*nshift+n])*(foft-insf[nbias+iframe*nshift+n]); */
/*      printf("gen: foft[%d]=%lf insf[%d]=%lf (Hz)\n",
           n,foft/PI2,n,insf[nbias+iframe*nshift+n]/PI2); */
    }                          /* ---- loop end for each sample --- */
}

}
