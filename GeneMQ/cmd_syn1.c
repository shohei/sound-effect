#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
/* #include "sinupktNew.h"
#include "sinupktfrtr.h" */
#define MAXARG 10
#define MAXCOLUMN 100
#define NFFTP1 4097
/*  ---------------------------------------------------------------
     Synthesis/Modification operation using sinusoidal model's packet
     syn:  sound synthesis according to the command
     coded by Naotoshi Osaka    May 20, 2000
     Modified Oct. 22. 2012
    -------------------------------------------------------------- */

int cmd_syn1(int narg,int n_int,FILE *input,FILE *output)
{   int i,j,ii,n,ie;
    int index;
    int rstsnd,rensnd;
    double freq0,mag0;
    double s_Theta0,s_Theta1,s_Omega0,s_Omega1,s_Mag0,s_Mag1;
    double deltadummy,insferr0;   // insferr0 is always 0.
    int lastiframe;
    int lastj1,iframedummy,idlastdummy;  // dummy: unnecessary, 
     // supplied only for the consistency of gensinu version from the textbook of list 6.5
    extern void *alocbuf(int unit,int nsize);
    extern void get_t(int j,int n_int,int npk,int iord[],struct trackpkt *tdum,
                      FILE *input);
    extern void gensinu3(int iframe,int jtrack,double nTheta0,double nTheta1,
                         double nOmega0,double nOmega1,double Mag0,double Mag1,int nwdw,
                         int iroute,double rbuf[],double rpbuf[],
                         int icf,int icm,int icd,int nshift,double sf,double PI2,double ssec);
    extern void gensinu(struct trackpkt *tbuf,double *a_Theta0,double *a_Theta1,
             double *a_Omega0,double *a_Omega1,double *a_Mag0,double *a_Mag1,
             double ssec,double PI2,double sf,
             double rbuf[NFFTP1],double rpbuf[],int nshift,int nfft,
             double *delta,int *lastj1,int iframe,int *idlast,
             double *insferr0,int nwdw,int iroute,FILE *output2,
             int index,int idw);
    if(narg==3) {
        sscanf(arg[1],"%d",&rstsnd);
        sscanf(arg[2],"%d",&rensnd);
    }
    else {
        rstsnd=0;
        rensnd=nframe-1;
    }
    printf("narg=%d rstsnd=%d nframe=%d rensnd=%d\n",narg,rstsnd,nframe,rensnd);
    printf("ssec=%lf PI2=%lf sf=%lf\n",ssec,PI2,sf);
    printf("nshift=%d nfft=%d\n",nshift,nfft);
    if((output=fopen(fn_output,"wb"))==NULL) {
        /* printf("ER soundfile open error\n"); */ return(-6);
    }

    /*  ******* M.A. ******** */
    y=calloc(sizeof(double),nbuf);
    rbuf =calloc(sizeof(double),NSHIFT);
    rpbuf=calloc(sizeof(double),NSHIFT);
    /*  ******* E. M.A. ******** */
    for(i=0; i<nbuf; i++) {
        y[i]=0.0;   /* buffer clear */
    }

    if(trackorder==0) {  /* ----- track order synthesis ------  */
      printf("cmd_syn1: This path is omiited for this version.\n");
      printf("          In order to use trackorder, use general sinued (using gensinu3 inside)\n");
      printf("          gensinu only supports frame-order wave padding, not back and forth.\n");
      return(-1);
     }
    else {               /* ----- frame order synthesis.  ----- */
        printf(" frame_order synthesis: rstsnd=%d rensnd=%d\n",rstsnd,rensnd);
        for(ii=lastiframe=0; ii<ndata; ii++) {
            if(frtr[ii].iframe >=rstsnd && frtr[ii].iframe <=rensnd) {
                get_t(ii,n_int,npk,iord,&tdum,input);
                /*    printf("bef gensinu: frtr[%d].iframe=%d  tdum.iframe=%d frtr[%d].jtrack=%d \n",
                           ii,frtr[ii].iframe,tdum.iframe,ii,frtr[ii].jtrack);  */
                             index=1; 
                             if((int)tdum.iroute==1 || (int)tdum.iroute==3)index=0;
                             s_Theta0=(double)tdum.nTheta0/pow(2.,16.);
                             s_Theta1=(double)tdum.nTheta1/pow(2.,16.);
                             s_Omega0=(double)tdum.nOmega0/pow(2.,8.);
                             s_Omega1=(double)tdum.nOmega1/pow(2.,8.);
                             s_Mag0=(double)tdum.Mag0;
                             s_Mag1=(double)tdum.Mag1;
                    if(ii>0 && lastiframe != frtr[ii].iframe){ for(n=0;n<nshift;n++){rbuf[n]=0.0;}}
                    gensinu(&tdum,&s_Theta0,&s_Theta1,
                             &s_Omega0,&s_Omega1,&s_Mag0,&s_Mag1,
                             ssec,PI2,sf,
                             rbuf,rpbuf,nshift,nfft,
                             &deltadummy,&lastj1,iframedummy,&idlastdummy,
                             &insferr0,nwdw,(int)tdum.iroute,output,
                             index,idw[2]) ;
                /* here, no intention to implement now */
                /*              icf[frtr[ii].jtrack],icm[frtr[ii].jtrack],
                              icd[frtr[ii].jtrack]); */
                if(ii%10000==0) {
                    printf("F.order-synthesis: ii=%d\n",ii);
                }
                for(n=0; n<nshift; n++) {
                //   y[frtr[ii].iframe*nshift+n] += rbuf[n];
                  y[frtr[ii].iframe*nshift+n] = rbuf[n];
                }
                freq0=(double)tdum.nOmega0/pow(2.,8.)/PI2;
                mag0 =tdum.Mag0;
            }  /* end of if (frtr[ii].iframe <= ... */
         lastiframe=frtr[ii].iframe; /* preserves the previous iframe */
        }   /* end of ii */
    }    /* end of else */
    /*  ******* M.A. ******** */
    x=NULL;
    printf("nbuf=%d\n",nbuf);
    x=alocbuf(sizeof(short),nbuf);
    for(n=0; n< nshift*nframe; n++) {
        x[n]=y[n];
    }
    ie=fwrite(x,sizeof(short),nshift*nframe,output);
    /*  ******* F.M. ******** */
    if(x!=NULL) {
        free(x);
        x=NULL;
    }
    if(y!=NULL) {
        free(y);
        y=NULL;
    }
    if(rbuf!=NULL) {
        free(rbuf);
        rbuf=NULL;
    }
    if(rpbuf!=NULL) {
        free(rpbuf);
        rpbuf=NULL;
    }

    fclose(output);
    return(0);
}
