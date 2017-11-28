/* ---------------------------------------------------------------
   Phase Vocoder by Portnoff's algorithm
                  Coded by Naotoshi Osaka
   --------------------------------------------------------------- */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pv.h"
short *x;
double *y;
/* --------------------------------------------------------------- */
int main(argc,argv)
int argc;
char * argv[];
{   double snrseg;
    int i,nframe,nfnsize;
    char *fnI,*fnO;
    extern int pv(char * fnI, char * fnO);
    extern void *alocbuf(int unit,int nsize);
    FILE *output,*output1;

    printf("pv  Portnoff's algorithm ver.0 (by N. Osaka)\n");
    if(argc==1) {
        printf(" Usage: pv I_filename O_filename\n");
         printf("  I_filename   : Input  sound file name\n");
         printf("  O_filename   : Output sound file name\n");
        return(0);
    }
    nfnsize=120;
    fnI = calloc(nfnsize,sizeof(char));
    fnO = calloc(nfnsize,sizeof(char));
    sscanf(argv[1],"%s",fnI);
    sscanf(argv[2],"%s",fnO);

    x=(short *)alocbuf(sizeof(short),NBUF);
    y=(double *)alocbuf(sizeof(double),NBUF);

    nframe=pv(fnI,fnO);
    printf(" nframe=%d \n",nframe);

    free(x);
    x=NULL;
    free(y);
    y=NULL;
    return 0;
}
/* -------------------------------------------------------------------
  Phase Vocoder (by Portnoff)  -- Analysis and synthesis:
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

      originally coded by Naotoshi Osaka  1993/9/27
     Modified Oct. Oct. 26, 2012
   ------------------------------------------------------------------- */
#include <stdio.h>
#include <math.h>
#include "pv.h"
#include "wavlib.h"
int pv(char * fnI, char * fnO)
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

    int *fftpos,prpos,iposn;
    int nfft,smsec,index,nwdw,nshift,nframe,i,j,k,ie,ie1,ndata,L,Q,R;
    int nana,nth,nsize,nleft,ist,adrsbias;
    int readbias;
    /* -------- local peak derivation related --------------- */
    char namein[200],nameout[200];
    FILE *input,*input1,*output,*output1,*output2;
    extern int fft(int n,double x[],double y[]);
    extern void erout(int ie);
    extern lpfdsn1(int N,int L,double wdw[],double f[]);
    extern int fsreadpv(short sbuf[],double rbuf[],int smsec,double sf,
                        int nfft,int *nleft,int L,int Q,FILE *input,int index);
    extern void hamwdw1(double wdw[],int n);
    void rotate(int ibuf[],int n,int index);
    extern double segmsn(double rorgbuf[],double rbuf[],int nsample);
    extern wdwsgnlpv(double rbuf[],double wdw[],int N,int L,
                     double wrbuf[],int nth);
    extern void *alocbuf(int unit,int nsize);
    extern void alocbuf2d_d(double ***dbuf,int nd1,int nd0);
    extern void freemem_d(double ***buf,int ndim1);
    extern FILE* wavRopen(wav_head *bufhead, char *fn);
    extern FILE* wavWopen(wav_head *bufhead, short Nch,long fs, 
                          int bit, int n_data, char *fn, int id);
    extern int wavWclose(wav_head *bufhead,int n_data,FILE *fp,int id);

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
  nfft=NFFTP1-1;
  smsec=SMSEC;
  // sf=SF;  // Necessary in case of raw input data
  sf=(double)bufhead.fs/1000.;
  nshift=(int)((double)smsec*sf);
  if(nshift>NSHIFT) {
        erout(0);
        return(0);
   } 
  printf("nshift=%d\n",nshift);
    L=LMAX;
    R=nshift;
    Q=QMAX;
    adrsbias= Q*R;
    /* (0) */
    /* window and low pass filter design  */
    hamwdw1(hwdw,(nwdw=2*L*nfft+1));
    hamwdw1(hwdw_int,(nwdw=2*Q*R+1));
    lpfdsn1(nfft,L,hwdw,lpf);
    lpfdsn1(R,Q,hwdw_int,lpf1);

    nana= 0-(Q-1)*R;
    for(i=prpos=0; i<2*Q; i++) {
        fftpos[i]=i;
    }
    for(i=index=ndata=iposn=0; ; i++) { /* --- Big loop for frame processing ------ */
        /* (1) */
        index = fsreadpv(sbuf,rbuf,smsec,sf,nfft,&nleft,L,Q,input,index);
        if(index== -2) {
            break;
        }
        /* M */ /*  for(j=0;j<2*L*nfft+1;j++){printf("rbuf[%4d]=%lf\n",j,rbuf[j]);} */
        rotate(fftpos,2*Q,-1);
        /*  for(j=0;j<2*Q;j++){printf("fp[%2d]=%2d",j,fftpos[j]);}
             printf(" iposn=%d prpos=%d \n",iposn,prpos);  */
        /*      for(j=0;j<2*L*nfft+1;j++){printf(" j=%d rbuf[j]=%lf\n",j,rbuf[j]);}  */
        /* (2) */
        if(nana>=0) {
            nth= (int)fmod((double)nana,(double)nfft);
        }
        else       {
            nth= nfft - (int)fmod((double)(-nana),(double)nfft);
        }
        //     printf(" i=%2d nana=%d nth=%d\n",i,nana,nth);
        wdwsgnlpv(rbuf,lpf,nfft,L,wr0buf,nth);
        /* if(i>=1){
           for(j=0;j< nfft;j++){printf(" j=%d wr0buf[j]=%lf\n",j,wr0buf[j]);}
        }  */
        /* (3) */
        for(j=0; j<nfft+1; j++) {
            wibuf[prpos][j]=0.0;   /* imaginary:=0 */
        }
        ie1=fft(nfft,wr0buf,wibuf[prpos]);
        /* pwr */
        for(j=0; j<=nfft/2; j++)
        {   if((powlin=wr0buf[j]*wr0buf[j]+wibuf[prpos][j]*wibuf[prpos][j])<=0) {
                mgbuf[j]= -100.0;
            }
            else mgbuf[j]=10.0*log10(powlin);
        }
        /*   for(j=0;j< nfft/2;j++){
                    printf("pwr[%4d]=%6.2lf  wr0buf[%4d]=%lf wibuf[%3d][%4d]=%lf\n",
                           j,mgbuf[j],j,wr0buf[j],prpos,j,wibuf[prpos][j]);}  */

        /* (4) */  for(j=0; j<nfft; j++) {
            wrbuf[prpos][j]=wr0buf[j];
        }
        fwrite(wr0buf,sizeof(double),nfft,output);
        fwrite(wibuf[prpos],sizeof(double),nfft,output);
        /* (5) */
        ie1=fft(-nfft,wrbuf[prpos],wibuf[prpos]);
        /*   if(i>=0){
            for(j=0;j< nfft;j++){printf(" wrbuf[%4d][%4d]=%lf wibuf[%3d][%4d]=%lf\n",
                 prpos,j,wrbuf[prpos][j],prpos,j,wibuf[prpos][j]);}
           }  */
        /* (6) */
        if(i>= 2*Q-1) {
            /* M */ /* for(k=0;k<2*Q;k++){
                       printf("wrbuf[%2d][%2d]=%lf \n",
                           fftpos[k],iposn,wrbuf[fftpos[k]][iposn]);
                     } */
            for(j=0; j<R; j++) { /* interpolation of R data */
                sum=0.0;
                ist= -(-Q+1)*R+j;
                for(k=0; k<2*Q; k++) { /* convolution */
                    sum += lpf1[adrsbias+ist-k*R]*wrbuf[fftpos[k]][iposn];
                    /*     printf(" lpf1[%4d]=%lf wrbuf[%2d][%3d]=%lf sum=%lf\n",
                                adrsbias+ist-k*R,lpf1[adrsbias+ist-k*R],fftpos[k],
                                iposn,wrbuf[fftpos[k]][iposn],sum);  */
                }
                synbuf[j]=(short)sum;
                /*       printf("synbuf[%3d]=%d\n",j,synbuf[j]);  */
                iposn++;
                if(iposn == nfft)iposn=0;
            }
            ie=fwrite(synbuf,sizeof(short),nshift,output1);
            ndata += ie;
            /*         ie=fwrite(rpbuf,sizeof(double),nshift,output1);   */
        }   /* end of if(i>=Q) */
        prpos++;
        if(prpos>=2*Q) {
            prpos=0;
        }
        nana += nshift;
        nframe=i+1;
    }  /*  Big loop end for i */

    printf(" nframe=%d\n",nframe);
    fclose(input);
    fclose(output);
    wavWclose(&bufhead,ndata,output1,1);
    /* ---------------------------Evaluation ---------------- */
    input=fopen(fnI,"r+b");
    input1=fopen(fnO,"r+b");
    output1=fopen("pvnoise","w+b");
    readbias=44; // headersize + alpha data are ignored (not signal)
    snrseg = 0.0;
    printf(" physical evaluation using segmental S/N.\n");
    fseek(input, (long)readbias,SEEK_SET);
    fseek(input1,(long)readbias,SEEK_SET);
    for(i=nframe=0;; i++) {
        ie =fread(sbuf,sizeof(short),nshift,input);
        ie1=fread(synbuf,sizeof(short),nshift,input1);
        for(j=0; j<ie; j++) {
            mgbuf[j]=(double)sbuf[j];
            mgdifbuf[j]=(double)synbuf[j];
        }
        snrsegfr= segmsn(mgbuf,mgdifbuf,ie);
        snrseg +=snrsegfr;
        printf(" i=%d ie=%d snrsegfr= %lf (dB)\n",i,ie,snrsegfr);
        nframe++;
        if(ie !=nshift) break;
        ie =fwrite(sbuf,sizeof(short),nshift,output1);
    }
    snrseg /= (double)nframe;
    printf(" SNseg=%lf(dB)\n",snrseg);
    fclose(input);
    fclose(input1);
    fclose(output1);

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
check(short sbuf[],double rbuf[],int smsec,double sf,int nfft,
      int *nleft,int L,int Q,FILE *input,int index)
{
    printf("entered check.\n");

}