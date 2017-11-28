#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include "sinupktNew.h"
/* ---------------------------------------------------------------
   Sinusoidal representation of signal
      Original ver.  2/ 9/1994 (from sinu.1.c)
      Ver. 42       11/18/1999 (from sinu.41.c)
            Coded by Naotoshi Osaka
   --------------------------------------------------------------- */
/* External variables (buffers only) */
/*  File definition */
/* ===   Rewrite file I/O information according to your use === */
static char *fdir_template;
/* ============================================================= */
static char fn_config[]="config.sys";
static char fn_in[]="SOUND";
static char fn_syn[]="_s";
static char fn_syndef[]="SOUND_s";
static char fn_pit[]=".pit";
static char fn_er[]=".hrm";
static char fn_env[]=".env";
static char fn_sinupkt[]=".pkt";
char *fn_temp;
char *fn_synth;
char *fn_input;
char *fn_output;
char *filename;
char *c_temp;
double **partfr,**partdb,**phase;
int **ffpk,**trackn;
double *trkpwr;
int *nharm,*ntr,*jdgtr;

double range,*envfr,*envdb,*envlin,*intfr;
/* sound data in memory */
short *x;
/* System parameters: defined in an include file. The present version
   define the falue in a file "config.sys"      */
int SMSEC,NFFTP1,NWDWVAL,NSHIFT,NWDW,NHARM,NFRAME,NHARMCAND,NBUF,NTRACK,
    THRESHLENGTH,STOPPER,MALE,LWDW0,PITCHST,PITCHEN;
double SF,PAI,THRESHDB,DELTA,THRESHUV;
/* --------------------------------------------------------------- */
main(argc,argv)
int argc;
char * argv[];
{
    double snrseg;
    double insferrmean;
    int i,ie,nframe,sigindex,nwdw,ncharsize;
    char *chtemp;
    /* ------------------------------- */
    int sinusoid(int sigindex,short x[],int nsize,double *insferrmean,
                 double *snrsegbuf);
    extern void *alocbuf(int unit,int nsize);
    extern int greetmes(int argc,char * argv[]);
    int sysparsetVar0(char *fntemp,char *ctemp);
    FILE *input,*output,*output1;

    ncharsize=100;
    NTRACK= -101; /* not used now. dummyvalue */
    /* -- global parameter --- */
    partfr= NULL;
    partdb= NULL;
    phase=  NULL;
    trkpwr= NULL;
    ffpk=   NULL;
    trackn= NULL;

    nharm=  NULL;
    ntr=    NULL;
    jdgtr=  NULL;

    trkpwr =NULL;
    envfr =NULL;
    envdb =NULL;
    envlin=NULL;
    intfr =NULL;
    fn_temp=alocbuf(sizeof(char),ncharsize);
    fn_synth=alocbuf(sizeof(char),ncharsize);
    fn_input=alocbuf(sizeof(char),ncharsize);
    fn_output=alocbuf(sizeof(char),ncharsize);
    fdir_template=alocbuf(sizeof(char),ncharsize*2);
    c_temp=alocbuf(sizeof(char),ncharsize);

    /* message output. miniture manual in case N. of argments is 1 */
    ie=greetmes(argc,argv);
    if(ie==0)return(0);
    /* ------------ read system parameters from "config.sys" -------------
                     working directory and others
                    *input will be open and closed
       ------------------------------------------------------------------- */
    printf("-2 fn_pit=%s fn_er=%s\n",fn_pit,fn_er);

    sysparsetVar0(fn_temp,c_temp);

    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++
       THIS x SHOULD NOT BE ALLOCATED, BUT GIVEN FROM OUTSIDE
       IN OTKINSHI for raw sound data.
       ++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
    x=alocbuf(sizeof(short),NBUF);

    /* global parameter allocation */
    partfr   =(double **)alocbuf(sizeof(double *),2);
    partfr[0]=(double *)alocbuf(sizeof(double),NHARM);
    partfr[1]=(double *)alocbuf(sizeof(double),NHARM);
    partdb   =(double **)alocbuf(sizeof(double *),2);
    partdb[0]=(double *)alocbuf(sizeof(double),NHARM);
    partdb[1]=(double *)alocbuf(sizeof(double),NHARM);
    phase    =(double **)alocbuf(sizeof(double *),2);
    phase[0] =(double *)alocbuf(sizeof(double),NHARM);
    phase[1] =(double *)alocbuf(sizeof(double),NHARM);
    ffpk     =(int **)alocbuf(sizeof(int *),3);
    ffpk[0]  =(int *)alocbuf(sizeof(int),NHARM);
    ffpk[1]  =(int *)alocbuf(sizeof(int),NHARM);
    ffpk[2]  =(int *)alocbuf(sizeof(int),NHARM);
    trackn    =(int **)alocbuf(sizeof(int *),2);
    trackn[0] =(int *)alocbuf(sizeof(int),NHARM);
    trackn[1] =(int *)alocbuf(sizeof(int),NHARM);

    /* ---------------------------------------------------------- */
    printf("-2.5 fn_pit=%s fn_er=%s fn_syn=%s\n",fn_pit,fn_er,fn_syn);

    sscanf(argv[1],"%d",&sigindex);
    if(sigindex==0) {
        sscanf(argv[2],"%s",fn_input);
        if(argc>=4) {
            sscanf(argv[3],"%s",fdir_template);
            if(argc==5)sscanf(argv[4],"%s",fn_syn);
        }
        printf("-2.6 fn_pit=%s fn_er=%s\n",fn_pit,fn_er);

        if(argc!=5) {
            strcpy(fn_synth,strcat(strcpy(fn_temp,fn_input),fn_syn));
        }

        printf("%s\n",fn_input);
        printf("%s\n",fdir_template);
        printf("%s\n",fn_synth);

        printf("-2.7 fn_pit=%s fn_er=%s fn_syn=%s\n",fn_pit,fn_er,fn_syn);

    } /* end of sigindex==0 */

    else if(sigindex == 1) { /* this is forthe case sound is on Memory */
        if((input=fopen(strcat(strcpy(fn_temp,fdir_template),fn_in),"rb"))==NULL) {
            printf("sinu 1: Er. in Opening file \n");
            return(0);
        }
        ie=fread((char *)x,sizeof(short),NBUF,input);
        fclose(input);
    }

    if(sigindex==1) {
        printf("nsize =%d\n",ie);
    }
    printf("sigindex=%d ie=%d\n",sigindex,ie);
    printf("-3 fn_pit=%s fn_er=%s\n",fn_pit,fn_er);

    nframe=sinusoid(sigindex,x,ie,&insferrmean,&snrseg);

    /*                 partfr[0],partdb[0],phase[0],ffpk[0],trackn[0]); */
    if(nframe<0) { /* error return */
        if(nframe== -11) {
            printf("envfd: Err. nenv too large.\n");
        }
        if(nframe== -12) {
            printf("splin3: Err. change the size of d (=NDMAX).\n");
        }
        if(nframe== -13) {
            printf("sinu: Err. make NFRAME larger in 'config.sys'\n");
        }
    }
    else {
        printf(" nframe=%d insferrmean=%lf snrseg=%lf \n",nframe,insferrmean,snrseg);
    }
    /*  fwrite(snrseg,sizeof(double),1,output);
     fwrite(insferrmean,sizeof(double),1,output1);
     fclose(output); fclose(output1);    */

    /* main related buffers */
    if(fdir_template!=NULL) {
        free(fdir_template);
        fdir_template=NULL;
    }
    if(fn_temp      !=NULL) {
        free(fn_temp);
        fn_temp=NULL;
    }
    if(fn_synth     !=NULL) {
        free(fn_synth);
        fn_synth=NULL;
    }
    if(fn_input     !=NULL) {
        free(fn_input);
        fn_input=NULL;
    }
    if(fn_output    !=NULL) {
        free(fn_output);
        fn_output=NULL;
    }
    if(c_temp      !=NULL) {
        free(c_temp);
        c_temp=NULL;
    }
    /* -- for safety -- */
    if(envfr !=NULL) {
        free(envfr);
        envfr=NULL;
    }
    if(envdb !=NULL) {
        free(envdb);
        envdb=NULL;
    }
    if(envlin!=NULL) {
        free(envlin);
        envlin=NULL;
    }
    if(intfr !=NULL) {
        free(intfr);
        intfr=NULL;
    }

    if(partfr) {
        for(i=0; i<2; i++) {
            if(partfr[i]) {
                free(partfr[i]);
                partfr[i]=NULL;
            }
        }
        free(partfr);
        partfr=NULL;
    }
    if(partdb) {
        for(i=0; i<2; i++) {
            if(partdb[i]) {
                free(partdb[i]);
                partdb[i]=NULL;
            }
        }
        free(partdb);
        partdb=NULL;
    }
    if(phase) {
        for(i=0; i<2; i++) {
            if(phase[i])  {
                free(phase[i]);
                phase[i]=NULL;
            }
        }
        free(phase);
        phase=NULL;
    }
    if(ffpk)  {
        for(i=0; i<3; i++) {
            if(ffpk[i])   {
                free(ffpk[i]);
                ffpk[i]=NULL;
            }
        }
        free(ffpk);
        ffpk=NULL;
    }
    if(trackn) {
        for(i=0; i<2; i++) {
            if(trackn[i]) free(trackn[i]);
            trackn[i]=NULL;
        }
        free(trackn);
        trackn=NULL;
    }

    if(trkpwr!=NULL) {
        free(trkpwr);
        trkpwr=NULL;
    }
    if(nharm !=NULL) {
        free(nharm);
        nharm=NULL;
    }
    if(ntr   !=NULL) {
        free(ntr);
        ntr=NULL;
    }
    if(jdgtr !=NULL) {
        free(jdgtr);
        jdgtr=NULL;
    }

    if(x!=NULL) {
        free(x);
        x=NULL;
    }

    return(0);
}
/* ===================================================================
     Sinusoidal model 2nd version  -- Analysis:
      See sinu.doc  for program documentation
      coded by Naotoshi Osaka  2000/5/24
   =================================================================== */
/* -------------------------------------------------------------------
     Sinusoidal model 3rd version  -- Analysis & Synthesis:
  Parameter Description:
   System Input parameters:
    partfr[]:(double) frequencies of partials in Hz
    partdb[]:(double) peak level in dB
    phase[]:(double)  phase in radian
    delta:(double)   track search window in frequency (Hz)
    ffpk[]:(int)
    nharm:(int)     number of partials

    sbuf: (short)   signal reading buffer from file
    rbuf: (double)  real signal for analysis (without window)
    hwdw: (double)  normalized hamming/blackman window
    wrbuf:(double)  window multiplied real data of signal
    wibuf:(double)  imaginary data of signal
    mgbuf:(double)  log magnitude of spectrum
   Model parameters:
    sf:   (double)  sampling frequency (kHz)
    smsec:(int)     frame shift interval (msec)
    tsec:(double)   frame analysis interval fft point
    ssec:(double)   frame shift interval  (sec)
    nfft: (int)     number of fft points
    nharmmax:(int)  number of maximum harmonics
    nhc:   (int)    number of harmonics candidates (=buffer size of sgnf)
    threshdb:(double) threshold from peak harmonics level. Within the level
                    local peak is considered as harmonics.
    mgdifbuf;(double) First, differencial of log magitude of spectrum. Then,
                      peak frequency position (starting from 0)
   Procedures:
   --(1)--- signal data read --------------------

fn_input is supplied with raw sound file name.
      coded by Naotoshi Osaka  1993/10/4
   ------------------------------------------------------------------- */
#include <stdio.h>
#include <math.h>
int sinusoid(int sigindex,short x[],int nsize,double *insferrmean,
             double *snrsegbuf)
{
    /* short sbuf[NFFTP1];
     double rbuf[NFFTP1],hwdw[NWDW],hwdw1[NWDW],rtmpbuf[NFFTP1];
     double wrbuf[NFFTP1],wibuf[NFFTP1],mgbuf[NFFTP1],mgdifbuf[NFFTP1]; */
    short *sbuf;
    double *rbuf,*hwdw,*hwdw1,*rtmpbuf;
    double *wrbuf,*wibuf,*mgbuf,*mgdifbuf;

    double sf,pitch,sum,cons,cons1,phasem,powlin;
    double pitchst,pitchen,pitch0,tempvar,erharm,er1harm,er2harm,er3harm;
    double blimit,peakpt,peakval,tpow,threshuv;
    int nfft,smsec,index,nwdw,nshift,nframe,nframemax,i,j,ie,ie1,ndata;
    int incnharm;
    int indexR,trackorder;
    int nleft,startp;
    int nwdw0,lwdw0,male,sptst,spten,uv,uvdummy;
    /* -------- local peak derivation related --------------- */
    int nharmmax,nhc,nharm0,idwest;
    double threshdb;
    double delta,deltadummy,PI,PI2,omegatau;

    short **sgnf;

    double *r1buf;
    double *rpbuf,*rp1buf;
    double *rorgbuf;

    double insferr;
    int k,n,idfin;
    double ssec,tsec;
    /* -------- local peak derivation related --------------- */
    int threshlength,nproceed;
    int ntrackmax;
    double Theta0,Theta1,Omega0,Omega1,Mag0,Mag1;
    double Theta0M1,Theta1M1,Theta0P1,Theta1P1;
    double snrseg,snrsegfr,pwrmax;
    int ii,jj,kkk,itemp,ntrall,itrack,iroute,isn,ipos,nbuf,npk,npk0,idw;
    int lastj1,idlast,tempbuf[2];
    /* -- envelope derivation related --- */
    int npt,nenv;
    int nenvmax,dummy;

    struct trackpkt tbuf,tbuf1;   /* for buffer only */
    char *chtemp;
    FILE *input,*output,*output1,*output2,*output4;
    FILE *output5,*output6,*output8;
    /* ----------------------------------------------------- */
    /* ----------------------------------------------- */
    extern int fsread_mem(short sbuf[],double rbuf[],int smsec,double sf,
                          int nfft,int *startp,int nsize,int index);
    extern int fsread(short sbuf[], double rbuf[],int smsec,double sf,int nfft,int *nleft,
                      FILE *input,int index,int nwdw);
    extern int fsread3_s(short sbuf[],double rbuf[],int smsec,double sf,int nfft,
                         FILE *input,int nwdw,int index);
    extern int envfd(double partfr[],double partdb[],int nharm,double envfr[],
                     double envdb[],double range,int nenvmax);
    extern void *alocbuf(int unit,int nsize);
    extern int ffpeakmat1(int nharmmax,int framepos,double delta);
    void gensinu(struct trackpkt *tbuf,double *a_Theta0,double *a_Theta1,
                 double *a_Omega0,double *a_Omega1,double *a_Mag0,double *a_Mag1,
                 double ssec,double PI2,double sf,
                 double rbuf[],double rpbuf[],int nshift,int nfft,
                 double *delta,int *lastj1,int iframe,int *idlast,
                 double *insferr0,int nwdw,int iroute,FILE *output2,
                 int index,int idw);
    extern double ceppit2(int nfft,int sptst,int spten,int nwdw0,double sf,
                          double wrbuf[],double wibuf[],double mgbuf[],double rbuf[],
                          double hwdw[], FILE *output4,double *peakval,
                          double *tpow,int *uv,double threshuv);
    /* The following  fsread*() necessary */
    /* extern int fsread1(),fsread3_s(); */
    extern void erout(int ie);
    /* extern void normhamwdw1(double wdw[],int n); */
    extern void normblkwdw(double wdw[],int n);
    extern int wdwsgnl1(double rbuf[],double hwdw[],int nfft,int nwdw,
                        double wrbuf[]);
    extern int fft(int n,double x[],double y[]);
    extern int sgn(double buf[],int nbuf,short sgnf1[],short sgnf2[],
                   short sgnf3[]);
    extern int sgnmod1(short sgnf1[],short sgnf2[],short sgnf3[],int nbuf,
                       double buf[]);
    extern void phasecorrect(double *phase,double PI2,double omega,
                             int nfft,double sf);
    extern int harmonize(double partfr[],double blimit,int nharm,double harmbf[],
                         double harmNbf[],double pitch0,double *pitchdt,
                         double *err,double *err1,double *err2,double *err3);
    extern int splin3(double rx[],double ry[],int nd,double x[],double y[],
                      int ndata);
    extern double segmsn(double rorgbuf[],double rbuf[],int nsample);
    /* --------------------------------------------------------
         Modification here: File  is speech
       -------------------------------------------------------- */
    /* =============  System parameter definition ============== */

    /* allocation */

    /*   allocate these */
    /* short orgbuf[NFFTP1];  */
    printf("0 fn_pit=%s fn_er=%s fn_input=%s\n",fn_pit,fn_er,fn_input);

    sbuf=alocbuf(sizeof(short),NFFTP1);
    rbuf=alocbuf(sizeof(double),NFFTP1);
    hwdw=alocbuf(sizeof(double),NWDW);
    hwdw1=alocbuf(sizeof(double),NWDW);
    rtmpbuf=alocbuf(sizeof(double),NFFTP1);
    wrbuf=alocbuf(sizeof(double),NFFTP1);
    wibuf=alocbuf(sizeof(double),NFFTP1);
    mgbuf=alocbuf(sizeof(double),NFFTP1);
    mgdifbuf=alocbuf(sizeof(double),NFFTP1);
    r1buf=alocbuf(sizeof(double),NFFTP1);
    rpbuf=alocbuf(sizeof(double),NSHIFT);
    rp1buf=alocbuf(sizeof(double),NSHIFT);
    rorgbuf=alocbuf(sizeof(double),NFFTP1);

    sgnf     =(short **)alocbuf(sizeof(short *),3);
    sgnf[0]  =(short *)alocbuf(sizeof(short),NHARMCAND);
    sgnf[1]  =(short *)alocbuf(sizeof(short),NHARMCAND);
    sgnf[2]  =(short *)alocbuf(sizeof(short),NHARMCAND);

    /* global parameter allocation */
    trkpwr= alocbuf(sizeof(double),NTRACK);
    nharm=  alocbuf(sizeof(int),NFRAME);
    ntr=    alocbuf(sizeof(int),NTRACK);
    jdgtr=  alocbuf(sizeof(int),NTRACK);
    printf("1 fn_pit=%s fn_er=%s fn_input=%s\n",fn_pit,fn_er,fn_input);

    /* (0) */
    idw=0;
    nfft=NFFTP1-1;
    smsec=SMSEC;
    sf=SF;
    PI= PAI;
    PI2=PI*2.0;
    nharmmax=NHARM;
    threshdb=THRESHDB;
    nhc=NHARMCAND;
    nframemax=NFRAME;
    delta=DELTA;
    nwdw= NWDWVAL;
    if(nwdw>= NFFTP1)nwdw= nfft-1;
    blimit= (sf*1000./2.0)*(5./6.); /* effective freq range (Hz) */
    nframe = STOPPER;
    printf(" nframe=%d\n",nframe);
    ndata = 4000;
    /*           ndata=2200; */
    /* --- pitch related parameters --- */
    /*   point = sf*1000/pit
         -> nwdw0 = sf* 1000/pit * N  : Number of one pitch period *N
         -> lwdw0 : 100/pit *N  */
    /* male:    0 ... male,       1... female
       male=0 lwdw0=40 pitchst=50  pitchen=250    threshuv=60.0
       male=1 lwdw0=10 pitchst=200 pitchen=1000.0 threshuv=80.0
       male=1 lwdw0=20 pitchst=100 pitchen=500.0  threshuv=80.0 */

    male=MALE;
    lwdw0=LWDW0;
    pitchst=(double)PITCHST;
    pitchen=(double)PITCHEN;
    threshuv=THRESHUV;
    printf("male=%d lwdw0=%d pitchst=%lf pitchen=%lf threshuv=%lf\n",
           male, lwdw0,pitchst,pitchen,threshuv);
    sptst=sf*1000./pitchen;
    spten=sf*1000./pitchst;
    nwdw0= (double)lwdw0*sf;
    printf(" nwdw0=%d\n",nwdw0);
    nshift=(int)((double)smsec*sf);
    if(nshift>NSHIFT) {
        erout(0);
        return(0);
    }

    idwest=0; /* 0:not write, 1: write */
    /* --- envelope related parameters --- */
    dummy=0.0;
    range = 100.0; /*  range(Hz) to derive envelope */
    nenvmax=(int)((sf*1000./2.)/range)+1;
    npt=64*(int)(sf/2.)/4;
    envfr=alocbuf(8,nenvmax);
    envdb=alocbuf(8,nenvmax);
    intfr =alocbuf(8,npt);
    envlin=alocbuf(8,npt);
    for(j=0; j<npt; j++) {
        intfr[j]=(sf*1000./2./(double)npt) * (double)j;
    }
    /* -- Original ssinu related parameters ------ */
    /* =============  System parameter definition ============== */
    /* (0) */
    npk=15;
    nbuf=NBUF;
    ssec=(double)smsec/1000.0;
    tsec=(double)(nfft)/(sf*1000.0);
    threshlength=THRESHLENGTH;
    ntrackmax=NTRACK;
    nproceed=0;
    trackorder=1;    /* frame order analysis */
    snrseg=0.0;
    isn=0;
    /* ==================== process start =============== */
    printf("2 fn_pit=%s fn_er=%s fn_input=%s\n",fn_pit,fn_er,fn_input);

    /* ---------- Input data definition/pseudo signal data creation ------ */
    printf("sinusoid: sigindex=%2d\n",sigindex);

    if (sigindex== 1) {
        /*  This is for the case of windows Otkinshi.
            raw sound file name (without .wav) should be stored
           in the vaiable 'fn_input' */
        /* Define fn_input */
    }
    else if(sigindex== 0) {
        /*      if((input=fopen(fn_input,"rb"))==NULL){ */
        if((input=fopen((chtemp=strcat(strcpy(fn_temp,fdir_template),fn_input)),"rb"))==NULL) {
            printf("sinu 0: Er. in Opening file(=%s) \n",fn_input);
            return(0);
        }
        printf("filename=%s\n",chtemp);
    }
    else if(sigindex == -1) {
        if((input=fopen(chtemp=strcat(strcpy(fn_temp,fdir_template),fn_in),"rb"))==NULL) {
            printf("sinu 1: Er. in Opening file \n");
            return(0);
        }
        printf("in filename=%s\n",chtemp);
        strcpy(fn_input,fn_in);
    }
    else if (sigindex == -2) {
        if((input=fopen(chtemp=strcat(strcpy(fn_temp,fdir_template),fn_syndef),"rb"))==NULL) {
            printf("input filename=%s fn_input=%s\n",chtemp,fn_input);
            printf("sinu 2: Er. in Opening file \n");
            return(0);
        }
        strcpy(fn_input,fn_syndef);
        printf("input filename=%s\n",chtemp);
    }
    /* -------------------- output data definition ----------------------- */
    if((output4=fopen(chtemp=strcat(strcat(strcpy(fn_temp,fdir_template),fn_input),
                                    fn_pit),"wb"))==NULL) {
        printf("sinu 5: Er. in Opening file \n");
        return(0);
    }
    printf("fn_pit=%s pitch filename=%s\n",fn_pit,chtemp);

    if((output5=fopen(chtemp=strcat(strcat(strcpy(fn_temp,fdir_template),fn_input),
                                    fn_er),"wb"))==NULL) {
        printf("sinu 6: Er. in Opening file \n");
        return(0);
    }
    printf("harm=%s er filename=%s\n",fn_er,chtemp);
    if((output8=fopen(chtemp=strcat(strcat(strcpy(fn_temp,fdir_template),fn_input),
                                    fn_env),"wb"))==NULL) {
        printf("sinu 7: Er. in Opening file \n");
        return(0);
    }
    printf("env filename=%s\n",chtemp);
    /* -- for ssinu -- */
    ipos=0;
    if(sigindex==0) {
        if((output6 =fopen(chtemp=strcat(strcpy(fn_temp,fdir_template),
                                         fn_synth),"wb"))==NULL) {
            printf("ssinu 4: Er. in Opening file \n");
            return(0);
        }
        printf("syntehsis filename=%s\n",chtemp);
    }

    else if((output6 =fopen(strcat(strcat(strcpy(fn_temp,fdir_template),fn_input),
                                   fn_syn),"wb"))==NULL) {
        printf("ssinu 4: Er. in Opening file \n");
        return(0);
    }

    if((output2=fopen(chtemp=strcat(strcat(strcpy(fn_temp,fdir_template),fn_input),
                                    fn_sinupkt),"wb"))==NULL) {
        printf("ssinu 6: Er. in Opening file \n");
        return(0);
    }
    printf("sinupkt filename=%s\n",chtemp);

    /* --- Output packet header ------ */
    npk0= (-1)*npk;
    ie=fwrite(&nframemax,sizeof(int),1,output2);
    ie=fwrite(&ntrackmax,sizeof(int),1,output2);
    ie=fwrite(&nframe,sizeof(int),1,output2);
    ie=fwrite(&ntrall,sizeof(int),1,output2);
    ie=fwrite(&npk0,sizeof(int),1,output2);
    ie=fwrite(&sf,sizeof(double),1,output2);
    ie=fwrite(&smsec,sizeof(int),1,output2);
    ie=fwrite(&nfft,sizeof(int),1,output2);
    ie=fwrite(&nwdw,sizeof(int),1,output2);
    ie=fwrite(&trackorder,sizeof(int),1,output2);
    ie=fwrite(&nproceed,sizeof(int),1,output2); /* Only this is not unknown now */
    ie=fwrite(&threshdb,sizeof(double),1,output2);
    ie=fwrite(&threshlength,sizeof(int),1,output2);
    ie=fwrite(nharm,sizeof(int),nframemax,output2);
    /*   ie=fwrite(trkpwr,sizeof(double),ntrall,output2);
     ie=fwrite(jdgtr,sizeof(int),ntrall,output2);*/ /* not necessary for frame order */


    /* ---- initial processing for envelope output ----- */
    ie=fwrite(&dummy, sizeof(int),1,output8);
    ie=fwrite(&npt, sizeof(int),1,output8);
    ie=fwrite(intfr, sizeof(double),npt,output8);
    /* ---- initial processing for synthesis ----- */
    for(n=0; n<nshift; n++) {
        r1buf[n]=rp1buf[n]=rbuf[n]=0.0;   /* buffer clear */
    }
    /*    ie= fsread3_s(orgbuf,rorgbuf,smsec,sf,nfft,input,nwdw,0); */

    /* ------------------------------  Analysis --------------------------- */
    /*           ie= fsread1(rtmpbuf,rbuf,smsec,sf,nfft,input,0); */
    for(i=indexR=idfin=0;; i++) {  /* ------ Big loop for frame processing ------ */
        if(i>nframemax) { /*  Err. nframe >= nframemax. */
            printf("nframe (=%d) >= nframemax(=%d).\n",i,nframemax);
            return(-13);
        }
        if(nframe<0) {
            if(i%100==0)printf("frame #=%d \n",i);
            /*  indexR= (i==0 ? 0 : 1); *//* This is only use for sig input */
            /* (1) */ /*  ie= fsread1(rtmpbuf,rbuf,smsec,sf,nfft,input,indexR); */
            if(sigindex==1) {
                indexR=fsread_mem(x,rbuf,smsec,sf,nfft,&startp,nsize,indexR);
                /*        if(indexR == -2){idfin=1; nframe=i; i++;  goto label2;} */
            }
            else {
                indexR = fsread(sbuf,rbuf,smsec,sf,nfft,&nleft,input,indexR,nwdw);
            }
            /*     for(j=0;j<nshift;j++){printf("%7.4lf ",rbuf[j]);} printf("\n");  */
            /* (2) */
            pitch0=ceppit2(nfft,sptst,spten,nwdw0,sf,wrbuf,wibuf,mgbuf,rbuf,hwdw,
                           output4,&peakval,&tpow,&uv,threshuv);
            if(i==0)printf("i=%d treat pitch as %6.0lf (Hz) nwdw=%d indexR=%d\n",
                               i,pitch0,nwdw,indexR);
            if(nwdw>NWDW) {
                erout(1);
                return(0);
            }
            /* (3) */ /* normhamwdw1(hwdw,nwdw); */ normblkwdw(hwdw,nwdw);
            /* (4) */
            wdwsgnl1(rbuf,hwdw,nfft,nwdw,wrbuf);

            /* (5) */
            for(j=0; j<nfft+1; j++) {
                wibuf[j]=0.0;   /* imaginary:=0 */
            }
            ie1=fft(nfft,wrbuf,wibuf);
            /*           fwrite(wrbuf,sizeof(double),nfft,output);
                       fwrite(wibuf,sizeof(double),nfft,output);  */
            /* (6) */
            /* 6.1 */
            for(j=0; j<=nfft/2; j++)
            {   /*powlin=wrbuf[j]*wrbuf[j]+wibuf[j]*wibuf[j];
                printf("j=%d  powlin=%lf wrbuf[j]=%lf wibuf[j]=%lf\n",
                 j,powlin,wrbuf[j],wibuf[j]); */
                if((powlin=wrbuf[j]*wrbuf[j]+wibuf[j]*wibuf[j])<=0.0)
                    mgbuf[j]= -100.0;
                else mgbuf[j]=10.0*log10(powlin);
            }
            /* 6.2 */  for(j=1; j<=nfft/2; j++) {
                mgdifbuf[j-1]=mgbuf[j]-mgbuf[j-1];
            }
            /* 6.3 */  nharm0=sgn(mgdifbuf,nfft/2,sgnf[0],sgnf[1],sgnf[2]);
            if(nharm0>nhc) {
                erout(2);
                return(0);
            }
            /* 6.4 */  nharm[i]=sgnmod1(sgnf[0],sgnf[1],sgnf[2],nharm0,mgdifbuf);
            if(nharm[i]>nharmmax) {
                erout(3);
                return(0);
            }
            /* M */ /*   printf("sinusoid: nharm[%3d]=%5d \n",i,nharm[i]);  */
            /*           for(j=0;j<nharm[i];j++)
                        {printf("f. mgdif[%3d]=%3.0lf %8.2lfHz %6.2lf dB\n ",
               j,mgdifbuf[j],mgdifbuf[j]*sf*1000./(double)nfft,mgbuf[(int)mgdifbuf[j]]);}*/

            for(j=0; j<nharm[i]; j++) {
                partfr[1][j]=mgdifbuf[j]*(sf*1000./(double)nfft);
                partdb[1][j]=mgbuf[(int)mgdifbuf[j]];
                phase[1][j]=
                    atan2(wibuf[(int)mgdifbuf[j]],wrbuf[(int)mgdifbuf[j]]);

                phasecorrect(&phase[1][j],PI2, PI2*partfr[1][j],
                             (nwdw/2*2 == nwdw ? nwdw: nwdw-1),sf);
                /*             omegatau = PI2*partfr[1][j]*((double)((nwdw-1)/2)/(sf*1000.));
                             phase[1][j]= fmod(phase[1][j]+omegatau,PI2);
                             if(phase[1][j]>PI)phase[1][j] -= PI2;  */
                /*           if(i==0 && j<3){
                             printf("after: partfr[%d][%d]=%lf phase[%d][%d]=%lf w_tau=%lf \n",
                               i,j,partfr[1][j],i,j,phase[1][j],omegatau);
                	       } */
            }
            /*    printf(" nharm=%d \n",nharm[i]);
                for(j=0;j<nharm[i];j++){printf(" %6.0lf ",mgdifbuf[j]);}printf("\n");
                for(j=0;j<nharm[i];j++){printf(" %6.1lf ",partfr[1][j]);}printf("\n");
                for(j=0;j<nharm[i];j++){printf(" %6.2lf ",partdb[1][j]);}printf("\n");
                for(j=0;j<nharm[i];j++){printf(" %6.3lf ",phase[1][j]);}printf("\n"); */
            /*      } */

            /* 6.6 */  /* precise pitch data extraction and voice-unvoiced decition */
            uvdummy= harmonize(partfr[1],blimit,nharm[i],wrbuf,wibuf,pitch0,&pitch,
                               &erharm,&er1harm,&er2harm,&er3harm);
            /*             fwrite(&pitch0,sizeof(double),1,output5);
                         fwrite(&pitch,sizeof(double),1,output5);
                         fwrite(&erharm,sizeof(double),1,output5);
                         fwrite(&er1harm,sizeof(double),1,output5);
                         fwrite(&er2harm,sizeof(double),1,output5);
                         fwrite(&er3harm,sizeof(double),1,output5);
                         fwrite(&peakval,sizeof(double),1,output5);
                         fwrite(&tpow,sizeof(double),1,output5);
            */

            /* (7) */
            ie=ffpeakmat1(nharmmax,i,delta);
            /* M */
            /*   for(j=0;j<nharm[i];j++){
               printf("i=%d ffpk[0][%d]=%d ffpk[1][%d]=%d ffpk[2][%d]=%d partfr[0]=%lf partfr[1]=%lf\n",
                i,j,ffpk[0][j],j,ffpk[1][j],j,ffpk[2][j],partfr[0][j],partfr[1][j]);
              }
            */

            /* (8) */ /* ---- envelope derivation --- */
            nenv=envfd(partfr[1],partdb[1],nharm[i],envfr,envdb,range,nenvmax);
            if(nenv<0) {
                return(-11);   /* error return */
            }
            if(nenv<=2) {
                for(j=0; j<npt; j++) {
                    envlin[j]=0.0;
                }
            }
            /* adhoc rule to avoid muon kukan */
            else if(envfr[0]==envfr[1] || envfr[1]==envfr[2] ||
                    envfr[2]==envfr[3]) {
                envfr[0]=10.;
                envfr[1]=sf*1000./2./4.;
                envfr[2]=sf*1000./2.;
                envfr[3]=sf*1000./2.-100.;
            }
            else {
                ie=splin3(envfr,envdb,nenv,intfr,envlin,npt);
                if(ie !=0)return(-12);
            }

            /* M */
            /*  printf("nenv=%3d envfr: \n",nenv);
                for(j=0;j<nenv;j++){printf(" %3.0lf ",envfr[j]);}printf("\n");
              printf(" envdb \n");
                for(j=0;j<nenv;j++){printf(" %3.0lf ",envdb[j]);}printf("\n"); */
            ie=fwrite(envlin, sizeof(double),npt,output8);
            /*  printf(" intfr\n");
                for(j=0;j<npt;j++){printf(" %3.0lf ",intfr[j]);}printf("\n");
                for(j=0;j<npt;j++){printf(" %3.0lf ",envlin[j]);}printf("\n");
            */
            /* (f) */ /* procedure for final process */
            /*   if(ie<0){nframe=i+1;} */        /* alternative with below */
            if(indexR== -2) {
                nframe=i+1;
            }
            /* This is necessary when used with INFILE input */

            /* --- change dB value into linear amplitude ---- */
            for(j=0; j<nharm[i]; j++) {
                partdb[1][j]=2.0*pow(10.0,partdb[1][j]/20.0);
            }
        } /* --- end of if nframe<0) ---  */

        /* ====== Synthesis start ====== */
label2:
        if(i>0) { /* skipps for the very first time */
            k=i-1; /* k: output packet frame counter */
            for(n=0; n<nshift; n++) {
                rbuf[n]=r1buf[n];
                rpbuf[n]=rp1buf[n];
                r1buf[n]=rp1buf[n]=0.0;
            }
            if(idfin != 1) {
                for(j=0; j<nharm[i]; j++) {
                    trackn[1][j]= -1;   /* initialize of trackn[1][*] */
                }
            }

            incnharm = 0;              /* incremental number for newly born packets */
            for(j=0; j<nharm[k]; j++) {     /* ---- loop for harmonics ---- */
                /* --- phase interpolate function --- */
                iroute= -100;
                if(k==0) { /* case k==0  jj: present maximum jtrack count */
                    trackn[0][j]=jj=j;
                }
                if(idfin==1) { /* The last frame (k==nframe-1)  all will die. */
                    iroute=0;
                    Omega0=partfr[0][j]*PI2;
                    Omega1=Omega0;
                    Theta0=phase[0][j];
                    Theta1=Theta0+Omega0*ssec;
                    Theta1=fmod(Theta1,PI2);
                    Mag0=partdb[0][j];
                    Mag1= Mag0; /* 0.0; */
                    tbuf.jtrack = trackn[0][j];
                    index=1;
                }
                else if(ffpk[1][j]<0) {  /* newly born ; needs additinal process later */
                    /*   printf(" ffpk[%d][%d]=%d partfr[%d][%d]=%lf\n",k,j,ffpk[1][j],k+1,-ffpk[1][j]-1,partfr[1][-ffpk[1][j]-1]); */
                    iroute=1;
                    Theta0=phase[0][j];
                    Theta1=phase[1][-ffpk[1][j]-1];
                    Omega0=partfr[0][j]*PI2;
                    Omega1=partfr[1][-ffpk[1][j]-1]*PI2;
                    Mag0=partdb[0][j];
                    Mag1=partdb[1][-ffpk[1][j]-1];
                    tbuf.jtrack = trackn[0][j]=jj;
                    trackn[1][-ffpk[1][j]-1]= jj;
                    index=0;
                }
                else if(ffpk[1][j]==0 ) { /* dead */
                    /*   if(trackn[0][j]>=0){ */   /* non 2-length track */
                    iroute=2;
                    Omega0=partfr[0][j]*PI2;
                    Omega1=Omega0;
                    Theta0=phase[0][j];
                    Theta1=Theta0+Omega0*ssec;
                    Theta1=fmod(Theta1,PI2);
                    Mag0=partdb[0][j];
                    Mag1=0.0;
                    tbuf.jtrack = trackn[0][j];
                    if(tbuf.jtrack < 0) tbuf.jtrack=jj;   /* THIS ERASES JTRACK<0 (1/10'99) */
                    index=1;
                    /*   }  */
                }
                else { /* --- normal process --- */
                    iroute=3;
                    Theta0=phase[0][j];
                    Theta1=phase[1][ffpk[1][j]-1];
                    Omega0=partfr[0][j]*PI2;
                    Omega1=partfr[1][ffpk[1][j]-1]*PI2;
                    Mag0=partdb[0][j];
                    Mag1=partdb[1][ffpk[1][j]-1];
                    tbuf.jtrack = trackn[0][j];
                    trackn[1][ffpk[1][j]-1]=trackn[0][j];
                    index=0;
                }
                /* if(Mag0>1000 || Mag1>1000){printf("iroute=%d Omega0=%lf\n",iroute,Omega0);} */
                if(Mag0==0.0 && Mag1==0.0)break;  /* if no amlitude, discard the data */

                tbuf.iframe = k;
                if((Mag0 !=0.0 || Mag1 !=0.0) && (idfin!=1 || trackn[0][j]!= -1)) {
                    gensinu(&tbuf,&Theta0,&Theta1,&Omega0,&Omega1,&Mag0,&Mag1,ssec,PI2,sf,
                            r1buf,rp1buf,nshift,nfft,&deltadummy,&lastj1,k,&idlast,&insferr,
                            nwdw,iroute,output2,index,idw);
                    ie=fwrite(&tbuf,sizeof(short),npk,output2);
                    nproceed++;
                }
                /* ------------ additional generation for newly born track --------- */
                /* -- signal is written in the previous frame -- */

                /* exeptional process */     /* newly born ; additional part */
                if((k!=0 && ffpk[1][j]<0 && idfin!=1)
                        ||  (k!=0 && ffpk[1][j]==0 && trackn[0][j]<0 && idfin!=1) ) {
                    iroute=5;
                    Omega1=partfr[0][j]*PI2;
                    Omega0=Omega1;
                    Theta1=phase[0][j];
                    Theta0=Theta1-Omega1*ssec;
                    Theta0=fmod(Theta0,PI2);
                    Mag0=0.0;
                    Mag1=partdb[0][j];
                    tbuf1.jtrack = trackn[0][j]; /* no touch for trackn[1][*] here */
                    if(ffpk[1][j]<0) {
                        jj++;
                    }
                    if(ffpk[1][j]==0 && trackn[0][j]<0) { /* THIS ERASES JTRACK<0) (1/10/'99) */
                        tbuf1.jtrack = jj;
                        jj++;
                    }
                    tbuf1.iframe = k-1;
                    incnharm++;

                    if((Mag0 !=0.0 || Mag1 !=0.0) && (idfin!=1 || trackn[0][j]!= -1)) {
                        gensinu(&tbuf1,&Theta0,&Theta1,&Omega0,&Omega1,&Mag0,&Mag1,ssec,PI2,sf,
                                rbuf,rpbuf,nshift,nfft,&deltadummy,&lastj1,k,&idlast,&insferr,
                                nwdw,iroute,output2,1,idw);
                        ie=fwrite(&tbuf1,sizeof(short),npk,output2);
                        nproceed++;
                    }
                }     /* -- end of exeptional process */
            }                         /* ---- loop end (j) for harmonics ---- */
            if(idfin != 1) {
                for(j=0; j<nharm[i]; j++) {
                    trackn[0][j]=trackn[1][j];   /* shift of trackn[1][] */
                }
            }

            /* -- s/n calculation --- */
            if(k<nframe-3) {
                isn++;    /* edge data is no good for chartest */
                snrsegfr = segmsn(rorgbuf,r1buf,nshift);
                snrseg += snrsegfr;
                *insferrmean +=insferr;
                /*     printf("Frame #%d                 insferr=%lf  snrsegfr=%lf \n \n",
                             i,insferr,snrsegfr);  */
            }
            /*      ie= fsread3_s(orgbuf,rorgbuf,smsec,sf,nfft,input,nwdw,1);  */
            if(k==0) {
                jj++;
            }
            if(k!=0) {
                for(n=0; n<nshift; n++) {
                    sbuf[n]= (short)rbuf[n];   /* use rbuf here */
                }
                ie=fwrite(sbuf,sizeof(short),nshift,output6);
                /*    ie=fwrite(rpbuf,sizeof(double),nshift,output1);   */
            }   /* end of if (k!=0)  */

            for(j=0; j<nharm[k]; j++)  {
                ffpk[0][j]=ffpk[1][j];
            }
            if(idfin !=1) {
                for(j=0; j<nharm[k+1]; j++) {
                    ffpk[1][j]=ffpk[2][j];
                    trackn[0][j]=trackn[1][j];
                }
            }
            nharm[k] += incnharm;

        } /* ----- end of if(i>0) ---- */
        if(idfin !=1) {
            for(j=0; j<nharm[i]; j++) {
                partfr[0][j]=partfr[1][j];
                partdb[0][j]=partdb[1][j];
                phase[0][j]=phase[1][j];
            }
        }

        /*  printf(" sinu: fr loop end:i=%d k=%d nframe=%d \n",i,k,nframe);
         printf("sinusoid: i=%d nharm[%3d]=%5d \n",i,k,nharm[k]);  */
        if(idfin == 1) {
            break;   /* This goes out of this loop */
        }
        if(indexR == -2) {
            idfin=1;    /* CAUTION ! BE CAREFUL for i++ */
            i++;
            goto label2;
        }
    }                      /* ------ Big loop end for frame processing (i) ---- */

    if(nframe<=nframemax)nharm[nframe]= STOPPER;
    if(nframe>nframemax)return(-13); /* error return */
    printf("nframe=%d nharm[nframe-1]=%d\n",nframe,nharm[nframe-1]);
    for(n=0; n<nshift; n++) {
        sbuf[n]=r1buf[n];   /* use r1buf here */
    }
    ie=fwrite(sbuf,sizeof(short),nshift,output6);
    /*    ie=fwrite(rp1buf,sizeof(double),nshift,output1);  */

    snrseg /= (double)(isn);
    *snrsegbuf = snrseg;
    *insferrmean /= (double)(isn);
    /*    printf("Nframe(for s/n)=%d insferrmean=%lf snrseg=%lf\n",
         isn,*insferrmean,snrseg); */
    tempbuf[0]= -10;
    tempbuf[1]= -10;
    ie=fwrite(tempbuf,sizeof(int),2,output2);
    /* fseek(output2,0L,seek_SET); */
    rewind(output2);

    /* --- Output packet header ------ */
    ie=fwrite(&nframemax,sizeof(int),1,output2);
    ie=fwrite(&ntrackmax,sizeof(int),1,output2);
    ie=fwrite(&nframe,sizeof(int),1,output2);
    ie=fwrite(&ntrall,sizeof(int),1,output2);
    ie=fwrite(&npk0,sizeof(int),1,output2);
    ie=fwrite(&sf,sizeof(double),1,output2);
    ie=fwrite(&smsec,sizeof(int),1,output2);
    ie=fwrite(&nfft,sizeof(int),1,output2);
    ie=fwrite(&nwdw,sizeof(int),1,output2);
    ie=fwrite(&trackorder,sizeof(int),1,output2);
    ie=fwrite(&nproceed,sizeof(int),1,output2);
    ie=fwrite(&threshdb,sizeof(double),1,output2);
    ie=fwrite(&threshlength,sizeof(int),1,output2);
    ie=fwrite(nharm,sizeof(int),nframemax,output2);
    /*    ie=fwrite(trkpwr,sizeof(double),ntrall,output2);
        ie=fwrite(jdgtr,sizeof(int),ntrall,output2); */
    fclose(output2);

    /* for(i=0;i<nframe;i++){
      printf("frame#=%3d ffpk: \n",i);
        for(j=0;j<nharm[i];j++){printf(" %3d ",ffpk[1][j]);}printf("\n");
      printf(" partfr \n");
        for(j=0;j<nharm[i];j++){printf(" %3.0lf ",partfr[1][j]);}printf("\n");
      printf(" partdb \n");
        for(j=0;j<nharm[i];j++){printf(" %3.0lf ",partdb[1][j]);}printf("\n");
      printf(" phase \n");
        for(j=0;j<nharm[i];j++){printf(" %7.4lf ",phase[1][j]);}printf("\n");
      } */
    /*  for(i=0;i<nframe;i++){
       printf(" ffpk[%d][1]=%3d partfr[%d][1]=%3.0lf phase[%d][1]=%7.4lf \n",
       i,ffpk[i][1],i,partfr[i][1],i,phase[i][1]);
    } */

    printf("0 nframe=%d nproceed=%d\n",nframe,nproceed);

    if(sigindex <=0 )fclose(input);
    /* fclose(output); fclose(output1); */
    fclose(output4);
    fclose(output5);
    fclose(output6);
    fclose(output8);

    /* free memory */
    if(sbuf !=NULL) {
        free(sbuf);
        sbuf=NULL;
    }
    if(rbuf !=NULL) {
        free(rbuf);
        rbuf=NULL;
    }
    if(hwdw !=NULL) {
        free(hwdw);
        hwdw=NULL;
    }
    if(hwdw1 !=NULL) {
        free(hwdw1);
        hwdw1=NULL;
    }
    if(rtmpbuf !=NULL) {
        free(rtmpbuf);
        rtmpbuf=NULL;
    }
    if(wrbuf !=NULL)  {
        free(wrbuf);
        wrbuf=NULL;
    }
    if(mgbuf != NULL) {
        free(mgbuf);
        mgbuf=NULL;
    }
    if(mgdifbuf !=NULL) {
        free(mgdifbuf);
        mgdifbuf=NULL;
    }
    if(r1buf != NULL) {
        free(r1buf);
        r1buf=NULL;
    }
    if(rpbuf !=NULL) {
        free(rpbuf);
        rpbuf=NULL;
    }
    if(rp1buf!=NULL) {
        free(rp1buf);
        rp1buf=NULL;
    }
    if(rorgbuf!=NULL) {
        free(rorgbuf);
        rorgbuf=NULL;
    }

    if(sgnf !=NULL) {
        for(i=0; i<3; i++) {
            if(sgnf[i] !=NULL) {
                free(sgnf[i]);
                sgnf[i]=NULL;
            }
        }
        free(sgnf);
        sgnf=NULL;
    }
    if(envfr !=NULL) {
        free(envfr);
        envfr=NULL;
    }
    if(envdb !=NULL) {
        free(envdb);
        envdb=NULL;
    }
    if(envlin!=NULL) {
        free(envlin);
        envlin=NULL;
    }
    if(intfr !=NULL) {
        free(intfr);
        intfr=NULL;
    }

    if(trkpwr!=NULL) {
        free(trkpwr);
        trkpwr=NULL;
    }
    if(nharm !=NULL) {
        free(nharm);
        nharm=NULL;
    }
    if(ntr   !=NULL) {
        free(ntr);
        ntr=NULL;
    }
    if(jdgtr !=NULL) {
        free(jdgtr);
        jdgtr=NULL;
    }

    return(nframe);
}
/* ----------------------------------------------------
         Read working directory and sys pars.
      coded by Naotoshi Osaka 3/11/'00
 ----------------------------------------------------- */
int sysparsetVar0(char *fntmp,char *ctemp)
{   int i,length;
    FILE *input;
    void getText(int length,char *stbuf,char *chbuf,FILE *input);
    length=200;
    /*  file opening and error return */
    if((input=fopen(fn_config,"ra"))==NULL) {
        return(-1);
    }
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"%s",fdir_template);
    printf(" dir=%s\n",fdir_template);
    /* -- SMSEC -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"SMSEC=%d",&SMSEC);
    printf("SMSEC=%d\n",SMSEC);
    /* -- SF -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"SF=%lf",&SF);
    printf("SF=%lf\n",SF);
    /* -- NFFTP1 -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"NFFTP1=%d",&NFFTP1);
    printf("NFFTP1=%d\n",NFFTP1);
    /* -- NWDWVAL -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"NWDWVAL=%d",&NWDWVAL);
    printf("NWDWVAL=%d\n",NWDWVAL);
    /* -- NSHIFT -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"NSHIFT=%d",&NSHIFT);
    printf("NSHIFT=%d\n",NSHIFT);
    /* -- NWDW -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"NWDW=%d",&NWDW);
    printf("NWDW=%d\n",NWDW);
    /* -- NHARM -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"NHARM=%d",&NHARM);
    printf("NHARM=%d\n",NHARM);
    /* -- NFRAME -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"NFRAME=%d",&NFRAME);
    printf("NFRAME=%d\n",NFRAME);
    /* -- NHARMCAND -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"NHARMCAND=%d",&NHARMCAND);
    printf("NHARMCAND=%d\n",NHARMCAND);
    /* -- NBUF -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"NBUF=%d",&NBUF);
    printf("NBUF=%d\n",NBUF);
    /* -- NTRACK -- dummy now! */
    /*    getText(length,fntmp,ctemp,input);
        sscanf(ctemp,"NTRACK=%d",&NTRACK);
       printf("NTRACK=%d\n",NTRACK); */
    /* -- PAI -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"PAI=%lf",&PAI);
    printf("PAI=%lf\n",PAI);
    /* -- THRESHDB -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"THRESHDB=%lf",&THRESHDB);
    printf("THRESHDB=%lf\n",THRESHDB);
    /* -- THRESHLENGTH -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"THRESHLENGTH=%d",&THRESHLENGTH);
    printf("THRESHLENGTH=%d\n",THRESHLENGTH);
    /* -- DELTA -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"DELTA=%lf",&DELTA);
    printf("DELTA=%lf\n",DELTA);
    /* -- STOPPER -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"STOPPER=%d",&STOPPER);
    printf("STOPPER=%d\n",STOPPER);
    /* -- MALE -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"MALE=%d",&MALE);
    printf("MALE=%d\n",MALE);
    /* -- LWDW0 -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"LWDW0=%d",&LWDW0);
    printf("LWDW0=%d\n",LWDW0);
    /* -- PITCHST -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"PITCHST=%d",&PITCHST);
    printf("PITCHST=%d\n",PITCHST);
    /* -- PITCHEN -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"PITCHEN=%d",&PITCHEN);
    printf("PITCHEN=%d\n",PITCHEN);
    /* -- THRESHUV -- */
    getText(length,fntmp,ctemp,input);
    sscanf(ctemp,"THRESHUV=%lf",&THRESHUV);
    printf("THRESHUV=%lf\n",THRESHUV);

    fclose(input);
    return(0);
}
/* ------------------------------------- */
void getText(int length,char *stbuf,char *chbuf,FILE *input)
{   int i;
    stbuf[0]=chbuf[0]='\0';
    for(i=0;; i++) {
        fgets(stbuf,length,input);
        sscanf(stbuf,"%s",chbuf);
        if(chbuf[0]!='%' && chbuf[0]!='\0' && chbuf[0]!='\n' )
            break;
    }
}
/* ----------------------------------------------------------------------
      ffpeakmat1
   Description:
      frame to frame peak match
   Input:
    delta:(double) search window (Hz)
    partfr,nharm,ffpk
   Output:
    ffpk[k][]
        negative:  born here. During process, -1 is first given beforhand.
         0:  the track dies there.
        non zero:  |ffpk[k][n]| points the count on partfr[k+1], which is on
                 the same track. The counter starts from 1 and not 0.
                 This is because 0 would have double meaning: track dead
                 and the pointer to the very first frequency.
    ffpk[k+1][]
          1000: not used.( for initiallization. should differ from 'found')
          found:(=1)  found as on a track of previous frequency.
          born:(=-1)  newly born
   Caution: Pointer output here start from 1. But "fcandf" inside does use
           pointer from 0.
   version 1: difference:
           do not allow track crossing. added three places (denoted as @@@)
              coded by Naotoshi Osaka     Oct. 1/1993
   ---------------------------------------------------------------------- */
int ffpeakmat1(int nharmmax,int framepos,double delta)
{   int nframe,i,j,k,kk,n,m,m1,icand,found,born,dead;
    double minidist,minidist1;
    int npartid,idw;
    extern int fcandfd(int nharm1,double partfr[],double partfr1[],int ffpk1[],
                       double delta,int found,int n,double *minid,int *npart);

    found=1;
    dead=0;
    born= -1;
    nframe=framepos+1;
    k=framepos-1;
    if(framepos==0) {
        for(n=0; n<nharm[0]; n++)ffpk[1][n]=born;
        return(0);
    }
    if(framepos==1) {
        kk=1;
    }
    else {
        kk=1;
    }
    for(n=0; n<nharm[k+1]; n++)ffpk[kk+1][n]=1000;
    for(n=0; n<nharm[k]; n++) {
        /* step 1 */
        if(framepos==2) {
            idw=1;
        }
        else {
            idw=0;
        }
        m=fcandfd(nharm[k+1],partfr[0],partfr[1],ffpk[kk+1],delta,found,
                  n,&minidist,&npartid);
        /*  printf("ffpeakmat: m=%d minidist=%lf npartid=%d\n",m,minidist,npartid);  */
        if(npartid==0) {
            ffpk[kk][n]= dead;   /*  (a) track dies */
        }
        /* step 2 */
        else {                               /*  (b) candidate */
            if(n==nharm[k]-1) {                /* edge */
                if(n==0 ||( (n>0)&& ffpk[kk][n-1]>0 && m>=ffpk[kk][n-1])) { /* @@@ */
                    ffpk[kk+1][m++]=found;
                    ffpk[kk][n] *= m;
                }
                else {
                    ffpk[kk][n]=dead;   /* @@@ */
                }
            } /* end of edge */
            else {
                icand=n;
                for(i=n+1; i<nharm[k]; i++) {
                    if((minidist1=fabs(partfr[0][i]-partfr[1][m]))<minidist) {
                        minidist=minidist1;
                        icand=i;
                    }
                }
                if(icand==n) {                /* (c) candidate; best */
                    if(n==0 ||( (n>0)&&  m>=abs(ffpk[kk][n-1]))) {              /* @@@ */
                        ffpk[kk+1][m++]=found;
                        ffpk[kk][n]*= m;
                    }
                    else {
                        ffpk[kk][n]=dead;   /* @@@ */
                    }
                }
                else {                        /*  (d) /(e)  */
                    m1=fcandfd(m-1,partfr[0],partfr[1],ffpk[kk+1],delta,found,
                               n,&minidist,&npartid);
                    if(m1<0) {
                        ffpk[kk][n]=dead;   /* (d) */
                    }
                    else {                                        /* (e) */
                        if(n==0 ||( (n>0)&&  m1>=abs(ffpk[kk][n-1]))) {          /* @@@ */
                            ffpk[kk+1][m1++]=found;
                            ffpk[kk][n]*= m1;
                        }
                        else {
                            ffpk[kk][n]=dead;   /* @@@ */
                        }
                    } /* (e) */
                }
            }
        }
    }                   /* end of big loop for n */
    /* step 3 */
    for(i=0; i<nharm[k+1]; i++) {
        if(ffpk[kk+1][i]!= found)ffpk[kk+1][i]= born;
    }
    return(0);
}
/* ------------------------------------------------------------
     gensinu
   Description:
     find phase difference, given parameters
   Input:
    Index: 0 .. generation by cubic function
           1 .. generation by 1st order function (= cos(wt+theta))
   Output:
   note: size    rbuf[NFFTP1]
            Coded by Naotoshi Osaka Oct.12/1993
   ------------------------------------------------------------ */
void gensinu(struct trackpkt *tbuf,double *a_Theta0,double *a_Theta1,
             double *a_Omega0,double *a_Omega1,double *a_Mag0,double *a_Mag1,
             double ssec,double PI2,double sf,
             double rbuf[],double rpbuf[],int nshift,int nfft,
             double *delta,int *lastj1,int iframe,int *idlast,
             double *insferr0,int nwdw,int iroute,FILE *output2,
             int index,int idw)

{   double PI,x,M,miu1,matrixup,matrixdown,t,Alpha,Beta,theta,delta1,delta0;
    double Theta0,Theta1,Omega0,Omega1,Mag0,Mag1;
    int n,j;
    static double THETA0=0.0,tsec;

    double deltaf,deltafrate,foft,insferr;
    double tempdata;
    int nbias,ie;

    Theta0= *a_Theta0;
    Theta1= *a_Theta1;
    Omega0= *a_Omega0;
    Omega1= *a_Omega1;
    Mag0= *a_Mag0;
    Mag1= *a_Mag1;
    PI=PI2/2.0;
    tsec=(double)nfft/(sf*1000.0);   /* N/(sf*1000) */

label1:
    x= ((Theta0 + Omega0*ssec-Theta1)
        +(Omega1-Omega0)*ssec/2.0)/(PI2);
    M= x-(M=(double)((int)x)) < M+1.0-x ? M :M+1.0 ;
    miu1=M-x;
    /* ----------------------------------- */
    deltaf  = (Omega1-Omega0)/PI2;
    deltafrate= deltaf/(sf*1000./(double)nfft);

    /* ----- End of modified process ----------------- */
    if(index==1) {
        for(n=0; n<nshift; n++) {  /* ---- loop for each sample ---- */
            t=(double)n/(sf*1000.0);
            theta=Theta0+Omega0*t;
            if(n==0 || n== nshift-1) {
            }
            rbuf[n] += (Mag0+(Mag1-Mag0)*(double)n/(double)nshift)
                       * cos(fmod(theta,PI2));
            rpbuf[n] = theta;
        }                          /* ---- loop end for each sample --- */
        insferr=0.0;
        foft=Omega0;
    }
    else {
        matrixup  =Theta1-Theta0-Omega0*ssec+M*PI2;
        matrixdown=Omega1-Omega0;
        Alpha=matrixup*(3.0/(ssec*ssec))-matrixdown/ssec;
        Beta =matrixup*(-2.0/(ssec*ssec*ssec))+matrixdown/(ssec*ssec);
        insferr =0.;
        nbias=(nwdw/2*2==nwdw ? nwdw/2 : (nwdw-1)/2);
        for(n=0; n<nshift; n++) {  /* ---- loop for each sample ---- */
            t=(double)n/(sf*1000.0);
            theta=Theta0+Omega0*t+Alpha*t*t+Beta*t*t*t;
            foft = Omega0+2.0*Alpha*t+3.0*Beta*t*t;
            rbuf[n] += (Mag0+(Mag1-Mag0)*(double)n/(double)nshift)
                       * cos(fmod(theta,PI2));
            rpbuf[n]  = theta;
            /* insf evaluation */
            insferr +=  0.;
            /*       (foft-insf[nbias+iframe*nshift+n])*(foft-insf[nbias+iframe*nshift+n]); */
            /*      printf("gen: foft[%d]=%lf insf[%d]=%lf (Hz)\n",
                       n,foft/PI2,n,insf[nbias+iframe*nshift+n]/PI2); */
        }                          /* ---- loop end for each sample --- */
    }
    insferr= sqrt(insferr/(double)nshift)/PI2;
    *insferr0=insferr;
    /* If below is changed, change npk, and sinupktNew.h */
    tbuf->nTheta0=(int)(Theta0*pow(2.,16.));
    tbuf->nTheta1=(int)(Theta1*pow(2.,16.));
    tbuf->nOmega0=(int)(Omega0*pow(2.,8.));
    tbuf->nOmega1=(int)(Omega1*pow(2.,8.));
    tbuf->Mag0=(short)Mag0;
    tbuf->Mag1=(short)Mag1;
    tbuf->iroute=(short)iroute;

}
