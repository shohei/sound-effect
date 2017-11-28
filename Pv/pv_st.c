/* ---------------------------------------------------------------
   Phase Vocoder by Portnoff's algorithm
     Originally on 12/2/1994
     Modified 2012
     Copyright (c) 2012 Naotoshi Osaka 
   --------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include "pv.h"

/* for both sinusoid and ssinu */
//short x[NBUF]; double y[NBUF],insf[NBUF];
short *x;
double *y,*insf;
/* --------------------------------------------------------------- */
int main(argc,argv)
int argc;
char * argv[];
{   double snrseg[5000];
    double insferrmean[5000];
    double stretch;
    int i,nframe;
    int nfnsize;
    char *fnI,*fnO;

    extern int pv(char * fnI, char * fnO, double stretch);
    FILE *output,*output1;

    extern void *alocbuf(int unit,int nsize);
    extern int fsreadpvr(short sbuf[],double rbuf[],double smsec,double sf,
		  int nfft,int *nleft,int L,int Q,FILE *input,int index);
//    extern int fsreadpvr(sbuf,rbuf,smsec,sf,nfft,nleft,L,Q,input,index);

    x=(short *)alocbuf(sizeof(short),NBUF);
    y=(double *)alocbuf(sizeof(double),NBUF);
    insf=(double *)alocbuf(sizeof(double),NBUF);

    printf("pv_st ver.0 (by N.Osaka)\n");
    if(argc==1) {
        printf(" Usage: pv_st [input wav file] [output wav file] [stretch const]\n");
        return(0);
    }
    nfnsize=120;
    fnI = calloc(nfnsize,sizeof(char));
    fnO = calloc(nfnsize,sizeof(char));
    sscanf(argv[1],"%s",fnI);
    sscanf(argv[2],"%s",fnO);
    sscanf(argv[3],"%lf",&stretch);

        nframe=pv(fnI,fnO,stretch);
        printf(" i=%d  nframe=%d \n",i,nframe);

    free(x);
    free(y);
    free(insf);
    return 0;
}
/* -------------------------------------------------------------------
     Phase Vocoder 1st version  -- Analysis and synthesis:
  Parameter Description:
    sbuf: (short)   signal reading buffer from file
    rbuf: (double)  real signal for analysis (without window)
    hwdw: (double)  normalized hamming window
    wrbuf:(double)  window multiplied real data of signal
    wibuf:(double)  imaginary data of signal
    mgbuf:(double)  log magnitude of spectrum
   Model parameters:
    sf:   (double)  sampling frequency (kHz)
    smsec:(int)     frame shift interval (msec)
    nfft: (int)     number of fft points

      coded by Naotoshi Osaka  1993/9/27
   ------------------------------------------------------------------- */
#include <stdio.h>
#include <math.h>
#include "pv.h"
#include "wavlib.h"
int pv(char * fnI,char * fnO,double stretch)
{
//short sbuf[LNFFTP1],synbuf[NSHIFT];
//double rbuf[L2NFFTP1],hwdw[L2NFFTP1],hwdw_int[Q2RP1],rtmpbuf[NFFTP1];
//double wrbuf[Q2MAX][NFFTP1],wibuf[Q2MAX][NFFTP1],wr0buf[L2NFFTP1];
//double mgbuf[NFFTP1],mgdifbuf[NFFTP1];
//double lpf[L2NFFTP1],lpf1[Q2RP1];
    wav_head bufhead;
    short *sbuf, *synbuf;
    double *rbuf,*hwdw,*hwdw_int,*rtmpbuf;
    double **wrbuf,**wibuf,*wr0buf;
    double *mgbuf,*mgdifbuf;
    double *lpf,*lpf1;
    double sf,sum,cons,cons1,phasem,powlin;
    double snrseg,snrsegfr,smsecana,temp,theta;
    double PI2;
    int *fftpos,prpos,iposn;
    int nfft,smsec,index,nwdw,nshift,nshiftorg,nframe,i,j,k,ie,ie1,ndata,L,Q,R;
    int nana,nanast,nanamod,nth,nsize,nleft,ist,adrsbias;
    int readbias;
    /* -------- local peak derivation related --------------- */
    FILE *input,*input1,*output,*output1,*output2;
    extern void *alocbuf(int unit,int nsize);
    extern void alocbuf2d_d(double ***dbuf,int nd1,int nd0);
    extern void freemem_d(double ***buf,int ndim1);
    extern void erout(int ie);
    extern void hamwdw1(double wdw[],int n);
    extern void lpfdsn1(int N,int L,double wdw[],double f[]);
    extern int fft(int n,double x[],double y[]);
    extern int fsreadpvr(short sbuf[],double rbuf[],double smsec,double sf,
		  int nfft,int *nleft,int L,int Q,FILE *input,int index);
    extern void wdwsgnlpv(double rbuf[],double wdw[],int N,int L,
               double wrbuf[],int nth);
    extern double segmsn(double rorgbuf[],double rbuf[],int nsample);
    extern int fft(int n,double x[],double y[]);
     extern FILE* wavRopen(wav_head *bufhead, char *fn);
    extern FILE* wavWopen(wav_head *bufhead, short Nch,long fs, int bit,
               int n_data,char *fn,int id);
    extern int wavWclose(wav_head *bufhead,int n_data,FILE *fp,int id);
    void rotate(int ibuf[],int n,int index);

/* =============  System parameter definition ============== */
    /* (0) */ /* Static array images */
//hwdw[L2NFFTP1],hwdw_int[Q2RP1],rtmpbuf[NFFTP1];
//double wrbuf[Q2MAX][NFFTP1],wibuf[Q2MAX][NFFTP1],wr0buf[L2NFFTP1];
//double mgbuf[NFFTP1],mgdifbuf[NFFTP1];
//double lpf[L2NFFTP1],lpf1[Q2RP1];
    fftpos=(int *)alocbuf(sizeof(int),Q2MAX);
    sbuf=(short *)alocbuf(sizeof(short),LNFFTP1);
    synbuf=(short *)alocbuf(sizeof(short),NSHIFT);
    rbuf=(double *)alocbuf(sizeof(double),L2NFFTP1);
    hwdw=(double *)alocbuf(sizeof(double),L2NFFTP1);
    hwdw_int=(double *)alocbuf(sizeof(double),Q2RP1);
    rtmpbuf=(double *)alocbuf(sizeof(double),NFFTP1);
    alocbuf2d_d((double ***)&wrbuf,Q2MAX,NFFTP1);
    alocbuf2d_d((double ***)&wibuf,Q2MAX,NFFTP1);
    wr0buf=(double *)alocbuf(sizeof(double),L2NFFTP1);
    mgbuf= (double *)alocbuf(sizeof(double),NFFTP1);
    mgdifbuf=(double *)alocbuf(sizeof(double),NFFTP1);
    lpf=(double *)alocbuf(sizeof(double),L2NFFTP1);
    lpf1=(double *)alocbuf(sizeof(double),Q2RP1);


/* ---------- process start ----------- */
  input = wavRopen(&bufhead,fnI);
  output =fopen("fftdata","w+b");
  output1 = wavWopen(&bufhead, bufhead.Nch, bufhead.fs,bufhead.bit,
                 bufhead.Nbyte/bufhead.bl_size, fnO, 1);
//  See "pv.h" for the definition of NFFTP or other Capital letter variables.
    PI2=M_PI*2;
    nfft=NFFTP1-1;
    smsec=SMSEC;
 // sf=SF;  // Necessary in case of raw input data
    sf=(double)bufhead.fs/1000.;
    /*           stretch=2.0; */
    /*           printf("stretch(double)?");
               scanf("%lf",&stretch); */
    stretch =(double)((int)((double)smsec*sf*stretch))
             /((double)smsec*sf);
    printf(" new stretch = %lf\n",stretch);
    smsecana= (double)smsec/stretch;
    nshift=(int)((double)smsec*sf);
    nshiftorg=(int)(smsecana*sf);
    if(nshift>NSHIFT) {
        erout(0);
        return(0);
    }
    if(nshiftorg>NSHIFT) {
        erout(0);
        return(0);
    }
    L=LMAX;
    R=nshift;
    Q=QMAX;
    printf("stretch = %lf smsec=%5d smsecana=%7.2lf nshiftorg=%5d\n",
           stretch,smsec,smsecana,nshiftorg);
    adrsbias= Q*R;
    /* (0) */     /* window and low pass filter design  */
    hamwdw1(hwdw,(nwdw=2*L*nfft+1));
    hamwdw1(hwdw_int,(nwdw=2*Q*R+1));
    lpfdsn1(nfft,L,hwdw,lpf);
    lpfdsn1(R,Q,hwdw_int,lpf1);
    /*           for(j=0;j<2*L*nfft+1;j++){lpf[j]=1.0;} */
    /*           for(j=0;j<2*Q*R+1;j++){lpf1[j]=1.0;}  */
    /* (2) */
    nana=    0-(Q-1)*nshiftorg;
    nanast=  0-(Q-1)*R;
    for(i=prpos=0; i<2*Q; i++) {
        fftpos[i]=i;
    }
    for(i=index=ndata=iposn=0; ; i++) { /* --- Big loop for frame processing ------ */
        /* (1) */ index =fsreadpvr(sbuf,rbuf,smsecana,sf,nfft,&nleft,L,Q,input,index);
        if(index== -2) {
            break;
        }
        rotate(fftpos,2*Q,-1);
        /* (2) */
        nanamod = nanast    ;  /* nanast- (nanast - nana);  */
        if(i%100==0) {
            printf(" i=%4d nana=%4d nanast=%4d nanamod=%4d\n",i,nana,nanast,nanamod);
        }
        if(nanamod>=0) {
            nth= (int)fmod((double)nanamod,(double)nfft);
        }
        else       {
            nth= nfft - (int)fmod((double)(-nanamod),(double)nfft);
        }
        wdwsgnlpv(rbuf,lpf,nfft,L,wr0buf,nth);
        /* M */
        /*   if(i>=1){
             for(j=0;j< nfft;j++){printf(" j=%d wr0buf[j]=%lf\n",j,wr0buf[j]);}
         } */
        /* (3) */
        for(j=0; j<nfft+1; j++) {
            wibuf[prpos][j]=0.0;   /* imaginary:=0 */
        }
        if( (ie1=fft(nfft,wr0buf,wibuf[prpos]))<0 ) {erout(4);return(ie1);}
        for(j=0; j<nfft; j++) { /* phase change */
            nanamod=nanast-nana;
            if(nanamod>=0) {
                nth= (int)fmod((double)nanamod,(double)nfft);
            }
            else       {
                nth= nfft - (int)fmod((double)(-nanamod),(double)nfft);
            }
            theta=(double)nth/(double)nfft*(double)j;
            theta= PI2*(theta- (double)((int)theta));
            temp=wr0buf[j] * cos(theta)-wibuf[prpos][j] * sin(theta);
            wibuf[prpos][j]=wr0buf[j] * sin(theta)+wibuf[prpos][j] * cos(theta);
            wr0buf[j]=temp;
        }
        /* pwr */   for(j=0; j<=nfft/2; j++)
        {   if((powlin=wr0buf[j] * wr0buf[j]+wibuf[prpos][j] * wibuf[prpos][j])<=0) {
                mgbuf[j]= -100.0;
            }
            else mgbuf[j]=10.0*log10(powlin);
        }
        /* M */
        /*  for(j=0;j< nfft/2;j++){ */
        /*              for(j=18;j< 22;j++){
                    printf("pwr[%4d]=%6.2lf  wr0buf[%4d]=%lf wibuf[%3d][%4d]=%lf\n",
                           j,mgbuf[j],j,wr0buf[j],prpos,j,wibuf[prpos][j]);}  */
        /* (4) */  for(j=0; j<nfft; j++) {
            wrbuf[prpos][j]=wr0buf[j];
        }
        fwrite(wr0buf,sizeof(double),nfft,output);
        fwrite(wibuf[prpos],sizeof(double),nfft,output);
        /* (5) */
        if((ie1=fft(-nfft,wrbuf[prpos],wibuf[prpos]))<0){erout(4); return(ie1);}
        /* (6) */
        if(i>= 2*Q-1) {
            for(j=0; j<R; j++) { /* interpolation of R data */
                sum=0.0;
                ist= -(-Q+1)*R+j;
                for(k=0; k<2*Q; k++) { /* convolution */
                    sum += lpf1[adrsbias+ist-k*R]*wrbuf[fftpos[k]][iposn];
                }
                synbuf[j]=(short)sum;
                iposn++;
                if(iposn == nfft)iposn=0;
            }
            ie=fwrite(synbuf,sizeof(short),nshift,output1);
            ndata+=ie;
        }   /* end of if(i>=Q) */
        prpos++;
        if(prpos>=2*Q) {
            prpos=0;
        }
        nanast += nshift;
        nana += nshiftorg;
        nframe=i+1;
    }  /*  Big loop end for i */

    printf(" nframe=%d\n",nframe);
    fclose(input);
    fclose(output);
    wavWclose(&bufhead,ndata,output1,1);
/* --------------------------- Final process  ---------------- */
    free(sbuf);
    free(synbuf);
    free(rbuf);
    free(hwdw);
    free(hwdw_int);
    free(rtmpbuf);
    freemem_d((double ***)&wrbuf,Q2MAX);
    freemem_d((double ***)&wibuf,Q2MAX);
    free(wr0buf);
    free(mgbuf);
    free(mgdifbuf);
    free(lpf);
    free(lpf1);
    free(fftpos);
    return(nframe);
}
/* -----------------------------------------------------
    rotate int array data
    input: ibuf[]  (int)
           n       (int)
    output: ibuf[]
    index:  >=0  right rotate
            < 0  left rotate
    example: case index==0   (right)
     before: idum=[ 1,3,5,7,9,10]
     after:  idum=[10,1,3,5,7,9 ]
             case index== -1 (left)
     before: idum=[ 1,3,5,7,9,10]
     after:  idum=[ 3,5,7,9,10,1]
        coded by Naotoshi Osaka   Dec.20/1994
   ----------------------------------------------------- */
void rotate(int ibuf[],int n,int index)
{   int i,idum;
    if(index>=0) { /* right rotate */
        idum=ibuf[n-1];
        for(i=0; i<n-1; i++) {
            ibuf[n-1-i]=ibuf[n-2-i];
        }
        ibuf[0]=idum;
    }
    else { /* left rotate */
        idum=ibuf[0];
        for(i=0; i<n-1; i++) {
            ibuf[i]=ibuf[i+1];
        }
        ibuf[n-1]=idum;
    }
}
