#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include "sinupktNew.h"
#include "sinupktfrtr.h"
#include "sinuedPar.h"
#define MAXARG 10
#define MAXCOLUMN 100
#define PAI 3.14159265358979323846
//#define NENV 5000
//#define NSHIFT 500
/* -----------------------------------------------------------------
    Several synthesis operation using sinusoidal model's packet
          coded by Naotoshi Osaka March/27/1995
   ----------------------------------------------------------------- */

/* ----- System buffer --------- */
short *x;
double *y, *rbuf,*rpbuf; /* allocated in cmd_syn1.c (used in syntesis) */

short *irdie; int *iord;
int *iord1;

/* really necessary memory */
struct trackpkt tdum;

 struct frtrpkt *frtr;
/* char arg[MAXARG][MAXCOLUMN]; */
  char **arg;  
/* allocated temporary buffer  */
 int *icf,*icm,*icd,*icf1,*icm1,*icd1;
 double *cd1;

 int *ibuf0,*fstframe0,*lstframe0;
 int *ntr,*ntr_new,*jdgtr,*nharm;
 double *fmean0,*magmean0;
 double *trkpwr;

int *iordtemp,*iordtemp1,*iord_new,*frtr_ifnew,*frtr_jtnew;
int *frtr_ifsub,*frtr_jtsub;
short *irdie_new,*irdie_sub;

/*  -------- System variables ------------- */
 int npk,nfft,nwdw,smsec,nframe,nframemax,ntrackmax,threshlength,
     nshift,ntrall,ndata,ndatamax,trackorder,rstbias;
 int ntrall_ref;
 int nbuf,ndim0,ndim1;
 int nframe_new,ipick,jtrackmax;
 unsigned int nn;
 int NENV=5000,NSHIFT=500;

 double sf,PI,PI2,ssec,tsec,pitch,snrseg; // defined in sysparsetVar
 double threshdb;
 double bitrate;
 double alpha; 
 char *fn_input,*fn_output;
 char *fntmp;
 char *fntmp0,*fntmp1;
 int idw[20];

 static char filepkt[]=".pkt";
 static char filepktout[]=".pkt.bf";
 static char fnsound[]="_s";
 static char fnharm[]=".tjf"; /* trajectory data in frame order */
 static char fnmat[]=".tjt";  /* trajectory in track order */
/* ================================================================== */
main(int argc,char * argv[])
{int ie;
 int geneMQ(int argc, char * argv[]);
 void free_sinumem(void);
 void sinuedErr(int ie);

 ie=geneMQ(argc,argv);
 free_sinumem();
 if(ie==0){return(0);}  else sinuedErr(ie);
 return(0);
}

/* ----------------------------------------------------------------------- */
int geneMQ(int argc,char *argv[])
{
int ie,i,j,ii,jj,tcount,
    n_int,iold,
    iifst,iilst;

int idum1,idum2;
int fstframemin,lstframemax;
int tprob,comcl,istart,ntrallcommon;
int twindow,stfreq,enfreq,stfreqL,enfreqL,iframeord,btemp;
double pwth,pwthdb,twindowsec,temp,pwuth,pwdth,atemp,
        pwuthL;
struct sinuedparam Qp;
struct trackpkt tbuf;
static char *cmname[]={"org ","f0  ","r","M sn","M ch","M fm","c","a",
               "--- ","rp","pic "," ",
               "org ","f0  ","t","fpt ","tp","ln","am","me"," "," "," "," ",
               "org ","f","p"," "," "," "," "," "," "," "," ",
               "--- "};
char nametemp[3][10],ctemp[40],cmdline[MAXCOLUMN];
  static char *cmnd[]={"ex","?","he","help","hp","ru","ls","ref","del","pic","e","cv",
                       "zed","syn","mat"};
  static char prompt[]="}} ";
  int ncomd=15;       /* adjust each time change command */
  int icomd,narg,rst,ren,rstl,renl,rstL,renL,rstlL,renlL;
  int rstsnd,rensnd;
/* ---------------------------------------------------------- */
       int  syn_greeting(int argc,char * argv[]);
       void sysparsetVar(int idw);
       void pktmonitor(int i,int idw,struct trackpkt tdum);
       int  datacr_init(int iframeord);
       int  readpkt(FILE  *input,int *iframeord,int n_int,int idw[]);
       void setdefval(struct sinuedparam *Qp,int *rstL,int *rst,int *rstlL,
            int *rstl,int *renL,int *ren,int *renlL,int *renl,
            double *pwuthL,double *pwuth,double *pwdth,double *pwthdb,
            int *stfreqL,int *stfreq,int *enfreqL,int *enfreq);
      void init_trkdt(int *ntrallcommon,int *fstframemin,int *lstframemax,int n_int,
           FILE *input);

//extern int cmd_pkedit(int *icf,int *icf1,int *icm,int *icm1,int *icd,
//                  int *icd1,double *cd1);
//       int cmd_ru(struct sinuedparam *Qp,int n_int,int *fstframemin,
//                  int *lstframemax,int *iframeord,FILE *input, FILE *outputpk);
       int cmd_ls(struct sinuedparam *Qp,int narg,int ntrallcommon,int n_int,
                  char *cmname[],char nametemp[][10],char ctemp[],
                  FILE *input);
//       int cmd_del(struct sinuedparam *Qp,int narg,int index);
//       int cmd_cv(int *ntrallcommon,int *fstframemin,
//           int *lstframemax,int n_int,FILE *input);
       int cmd_zed(int *ntrallcommon,int n_int,int *fstframemin,
                   int *lstframemax,FILE *input);
       int cmd_syn1(int narg,int n_int,FILE *input,FILE *output);
//       int cmd_mat(int narg,int n_int,FILE *input,FILE *output1,FILE *outmat);
extern void *alocbuf(int unit,int nsize);
extern void alocbuf2d_d(double ***dbuf,int nd1,int nd0);
extern void alocbuf2d_i(int    ***ibuf,int nd1,int nd0);
extern void alocbuf2d_s(short  ***sbuf,int nd1,int nd0);
extern void alocbuf2d_c(char   ***cbuf,int nd1,int nd0);
extern void   freemem_d(double ***buf,int ndim1);
extern void   freemem_i(int    ***ibuf,int ndim1);
extern void   freemem_s(short  ***sbuf,int ndim1);
extern void   freemem_c(char   ***cbuf,int ndim1);
extern void get_cmd(char prompt[],char cmdline[],int maxline);
extern int  commandanal2(char cmdline[],char *cmnd[],int ncomd,char ***arg,
               int *icomd);
extern void  fmagmeanfd3(struct trackpkt tdum,struct frtrpkt frtr[],int ntrall,
        double fmean[],double magmean[],int fstframe[],int lstframe[],int ntr[],
        int *fstframemin,int *lstframemax,int n_int,int npk,int iord[],
        FILE *input,short irdie[],int index);
extern void cmd_hp(int narg,char *cmnd[],char ***arg);
extern void get_t(int j,int n_int,int npk,int iord[],struct trackpkt *tdum,
       FILE *input);
extern void get_trand(int j,int jold,int n_int,int npk,int iord[],
       struct trackpkt *tdum,FILE *input);

extern int sysparsetCom(FILE *input,int *nframemax,int *ntrackmax,int *nframe,
       int *ntrall,int *npk,double *sf,int *smsec,int *nfft,int *nwdw,
       int *trackorder,int *ndata,double *threshdb,int *threshlength,
       int **nharm,int **ntr,double **trkpwr,int **jdgtr,int idw);

extern void ordr(double data[],int n,int out[]);
extern void gensinu3(int iframe,int jtrack,double nTheta0,double nTheta1,
         double nOmega0,double nOmega1,double Mag0,double Mag1,int nwdw,
         int iroute,double rbuf[],double rpbuf[],
         int icf,int icm,int icd,int nshift,double sf,double PI2,double ssec);
/* extern void pktmix1(struct trackpkt t0[],int ntrk0,int fstf0,int lstf0,
       struct trackpkt t1[],int ntrk1,int fstf1,int lstf1,
       struct trackpkt t_out[],int *ntrk_out,int *fstf_out,int *lstf_out,
      double ssec,double magmean0,double magmean1,int index); */
extern double fpeakinwdw1(int twindow,double fwindow,int ntr[],double fmean[],
       double magmean[],int fstframe[],int lstframe[],
       int nframe,int ntrall,int rst,int *peakpos);

FILE *input,*input1,*output,*output1,*outputpk,*outmat;

/* ***********************  Process starts ******************************** */
  if(argc != 3) {
    printf("\nusage: geneMQ pkt_data filename\n");
    printf("   pkt_data   : Sinusoidal model related packet data. (for data format see readme)\n");
    printf("   filename   : file name of sound\n");
    printf(" note: if no._of_data is given in negative,\n");
    printf("        output file is 'raw' format. ");
    printf("Otherwize 'wav' format\n");
    return 0;
  }
  fn_input=alocbuf(sizeof(char),MAXCOLUMN);
  fn_output=alocbuf(sizeof(char),MAXCOLUMN);
  fntmp=alocbuf(sizeof(char),MAXCOLUMN*2);
sscanf(argv[1],"%s",fn_input);
sscanf(argv[2],"%s",fn_output); 
printf("fn_input=%s\n",fn_input);
printf("fn_output=%s\n",fn_output);

 if(ie=syn_greeting(argc,argv)==0)return(0); /* --- Greetings  to terminal--- */

printf("fn_input=%s\n",fn_input);
printf("fn_output=%s\n",fn_output);

  alocbuf2d_c((char ***)&arg,MAXARG,MAXCOLUMN); /* Memory alloc. for command line  */
  if((input=fopen(fn_input,"rb"))==NULL){  printf("filename=%s\n",fn_input);
return(-2);}
   
  n_int=sysparsetCom(input,&nframemax,&ntrackmax,&nframe,&ntrall,&npk,&sf,
             &smsec,&nfft,&nwdw,&trackorder,&ndata,&threshdb,&threshlength,
             (int **)&nharm,(int **)&ntr,(double **)&trkpwr,(int **)&jdgtr,
             idw[0]);                   /* --- header info read  from pkt file --- */
  sysparsetVar(idw[1]);                        /* ---OP: system variables  set --- */
  if(ie=readpkt(input,&iframeord,n_int,idw)!=0)return(ie); /* read packet */
  printf("trackorder=%d\n",trackorder);
/* ------------------------------------------------------------------- */
/* --- Initial data correction in case of frame order */
 if(trackorder==1 ){
 ie = datacr_init(iframeord);
 if(ie!=0) return(ie); }
 setdefval(&Qp,&rstL,&rst,&rstlL,&rstl,&renL,&ren,&renlL,&renl,
           &pwuthL,&pwuth,&pwdth,&pwthdb,   &stfreqL,&stfreq,&enfreqL,&enfreq);
                                                    /* OP: set default value */
label0:
init_trkdt(&ntrallcommon,&fstframemin,&lstframemax,n_int,input); /* OP: trkdata init */


/* --------------------------------------------------------------- */
    if((input=fopen(fn_input,"rb"))==NULL){return(-2);}
    printf("filename=%s\n",fn_input);
    n_int=sysparsetCom(input,&nframemax,&ntrackmax,&nframe,&ntrall,&npk,&sf,
             &smsec,&nfft,&nwdw,&trackorder,&ndata,&threshdb,&threshlength,
             (int **)&nharm,(int **)&ntr,(double **)&trkpwr,(int **)&jdgtr,
             idw[0]);       /* --- header info read  from pkt file --- */
  printf("pass 4\n");
    sysparsetVar(idw[1]);         /* ---OP: system variables  set --- */
    if(ie=readpkt(input,&iframeord,n_int,idw)!=0)return(ie); /* read packet */
 printf("pass 5\n");
/* --Initial data correction for frame order is not neceesary here. */
    setdefval(&Qp,&rstL,&rst,&rstlL,&rstl,&renL,&ren,&renlL,&renl,
           &pwuthL,&pwuth,&pwdth,&pwthdb,&stfreqL,&stfreq,&enfreqL,&enfreq);
                                                /* OP: set default value */
 printf("pass 6\n");
    init_trkdt(&ntrallcommon,&fstframemin,&lstframemax,n_int,input); 
   printf("pass 7\n");                                                   /* OP: trkdata init */
 
// case 6:  /* ls command */
    if(ie = cmd_ls(&Qp,narg,ntrallcommon,n_int,cmname,nametemp,ctemp,input) !=0) return(ie);
//    break;
  printf("pass 8\n");
// case 12: /* zed command */
    if(ie = cmd_zed(&ntrallcommon,n_int,&fstframemin,&lstframemax,input)!=0)
         return(ie);
//    break;
 printf("pass 9\n");
// case 13: /* syn command */ 
    if(ie = cmd_syn1(narg,n_int,input,output) !=0) return(ie);
//    break;
  printf("pass 10\n");
return(0);
}
//#include "cmd_ru.c"
#include "cmd_ls.c"
//#include "cmd_del.c"
//#include "cmd_cv.c"
#include "cmd_zed.c"
#include "cmd_syn1.c"
#include "genesinu.c"
//#include "cmd_mat.c"
/* ===================================================================== */
/* ---------------------------------------------------------
 syn_greeting
  Description:
   make greeting messages to let user know how to use the system
       coded by Naotoshi Osaka    2/16/1994
   --------------------------------------------------------- */
int syn_greeting(int argc,char * argv[])
{ int i,ibias;
 if(argc==1){
  printf(" ============================================================\n");
  printf(" sinued ver 43  4/30/00 (<sinued ver.42 2/7/00) (by N. Osaka)\n");
  printf(" Packet itself is not on the memory. \n");
  printf(" ============================================================\n");
  printf("\n Write working directory into 'config.syn.sys'\n");
   printf(" Usage: %s [-1/-2/-3] [input_file] [trackorder/dummy] [system var. m.] [packet m.] [ntr m.]",
          argv[0]);
   printf(" [renumb_trk m.] ]\n");
   printf("  where The 1st (and 2nd argument): \n");
   printf("       -1: trackorder is manually pointed in the next argument.\n");
   printf("       -2: trackorder is already written. \n");
   printf("       -3: the same as -2, moreover outputs datafile of trajectory.\n");
   printf(" Try \n  sinued -2 [file_n] 0 0 \n and trackorder value is known.\n");
   printf("  where frame/track order  = 1/0\n");
   printf("        m.= monitor on/off = 1/0 \n");
            return(0);
        }
 ibias=0;
 sscanf(argv[1],"%d",&idw[0]);
 printf("idw[0]=%d\n",idw[0]);
 if(idw[0] == -1 ){
 sscanf(argv[2],"%d",&trackorder);
 ibias=1;
 printf(" trackorder=%d\n",trackorder);
 }
 for(i=1;i<argc-2;i++){
  sscanf(argv[i+2+ibias],"%d",&idw[i]);
 }
 return(1);
}
/* -------------------------------------------------------
  setdefval.c
  set default value
    April 30, 2000      coded by Naotoshi Osaka
   ------------------------------------------------------- */
void setdefval(struct sinuedparam *Qp,int *rstL,int *rst,int *rstlL,int *rstl,
        int *renL,int *ren,int *renlL,int *renl,
        double *pwuthL,double *pwuth,double *pwdth,double *pwthdb,
        int *stfreqL,int *stfreq,int *enfreqL,int *enfreq)
 {
  *rstL= *rst = *rstlL= *rstl = *pwuthL= *pwuth = *stfreqL= *stfreq  =  0.;
   *renL= *ren = ndata+10;
   *renlL= *renl = 1000000;
   *pwdth=32767.; *pwthdb=20.0*log10(*pwdth);
   *enfreqL= *enfreq= 100000.;
   Qp->rstL = *rstL;
   Qp->rst = *rst;
   Qp->rstlL = *rstlL;
   Qp->rstl = *rstl;
   Qp->renL = *renL;
   Qp->ren =  *ren;
   Qp->renlL =  *renlL;
   Qp->renl =  *renl;
   Qp->pwuthL = *pwuthL;
   Qp->pwuth = *pwuth;
   Qp->pwdth = *pwdth;
   Qp->pwthdb = *pwthdb;
   Qp->stfreqL = *stfreqL;
   Qp->stfreq = *stfreq;
   Qp->enfreqL = *enfreqL;
   Qp->enfreq = *enfreq;
    ipick=1;
 }
/* -------------------------------------------------------------------
    init_trkdt.c
    initiallize track related data.
             May 1, 2000     Coded by Naotoshi Osaka
   ------------------------------------------------------------------- */
void init_trkdt(int *ntrallcommon,int *fstframemin,int *lstframemax,int n_int,FILE *input)
{
extern void *alocbuf(int unit,int nsize);
extern void  fmagmeanfd3(struct trackpkt tdum,struct frtrpkt frtr[],int ntrall,
        double fmean[],double magmean[],int fstframe[],int lstframe[],int ntr[],
        int *fstframemin,int *lstframemax,int n_int,int npk,int iord[],
        FILE *input,short irdie[],int index);
int i;

if(trackorder==0){ /* trackorder */
/*   Default value */
  printf("init-trkdt: ndata=%d size=%d ntrall=%d\n",
      ndata,sizeof(short)*npk,ntrall); 
  *ntrallcommon=ntrall_ref=ntrall; 

  icf =alocbuf(sizeof(int),ntrall);
  icm =alocbuf(sizeof(int),ntrall);
  icd =alocbuf(sizeof(int),ntrall);
  icf1=alocbuf(sizeof(int),ntrall);
  icm1=alocbuf(sizeof(int),ntrall);
  icd1=alocbuf(sizeof(int),ntrall);
   cd1=alocbuf(sizeof(double),ntrall);
  /* ------------------------------- */
   for(i=0;i<ntrall;i++){icf[i]=icm[i]=icd[i]=0; 
   }
fstframe0=alocbuf(sizeof(int),ntrall);
lstframe0=alocbuf(sizeof(int),ntrall);
fmean0=alocbuf(sizeof(double),ntrall);
magmean0=alocbuf(sizeof(double),ntrall);
  fmagmeanfd3(tdum,frtr,ntrall,fmean0,magmean0,fstframe0,lstframe0,ntr,
              fstframemin,lstframemax,n_int,npk,iord,input,irdie,0);
 } /* end of trackorder */
}
/* ----------------------------------------------------
   sysparsetVar
  Description: system parameter input from disk file
    coded by Naotoshi Osaka 2/16/1994 -> 5/22/1999
   ---------------------------------------------------- */
void sysparsetVar(int idw)
{
    PI=PAI;
    PI2=2.0*PI;
    ssec=(double)smsec/1000.0;
    tsec=(double)(nfft)/(sf*1000.0);
    nshift=(int)((double)smsec*sf);
   printf("sysparsetVar: PI=%lf ssec=%lf tsec=%lf nshift=%d\n",
   PI,ssec,tsec,nshift);
/*  double pitch,snrseg are not inherited from the packet file */
           pitch=220.0;        /* an example */
           nbuf=nshift*(nframe+10); /* somehow bigger size for safety */
           ndatamax= ndata;
           snrseg=0.0;
    printf("nframemax=%d\n",nframemax);
   printf("out sysparsetVar pitch=%lf nbuf=%d ndatamax=%d\n",pitch,nbuf,ndatamax);
}
/* ----------------------------------------------------------
   pktmonitor
  Description:
   pkt monitoring
              coded by Naotoshi Osaka 2/16/1994
   ---------------------------------------------------------- */
void pktmonitor(int i,int idw,struct trackpkt tdum)
 { double PI2;
  if(idw==1){
     PI2 = 2.*PAI;
     printf("%4d tdum.iframe=%3d tdum.jtrack=%4d iroute=%2d\n",
       i,tdum.iframe,tdum.jtrack,tdum.iroute);
     printf("      nTheta0=%6.3lf nTheta1=%6.3lf  \n",
       (double)tdum.nTheta0/pow(2.,16.),(double)tdum.nTheta1/pow(2.,16.));
     printf("      nOmega0/2PI=%6.0lf[Hz] nOmega1=%6.0lf[Hz] Mag0=%6d Mag1=%6d \n",
       (double)tdum.nOmega0/pow(2.,8.)/PI2,(double)tdum.nOmega1/pow(2.,8.)/PI2,
        tdum.Mag0,tdum.Mag1);
  }
 }
#include "datainit.c"
#include "readpkt.c"
/* ---------------------------------------------------------------
    free sinu related memory
       dependent software for main
      coded by Naotoshi Osaka March 28,'00
   --------------------------------------------------------------- */
void free_sinumem(void)
{
/* memory management */
if(icf!=NULL){free(icf);icf=NULL;}
if(icm!=NULL){free(icm); icm=NULL;}
if(icd!=NULL){free(icd); icd=NULL;}
if(icf1!=NULL){free(icf1);icf1=NULL;}
if(icm1!=NULL){free(icm1);icm1=NULL;}
if(icd1!=NULL){free(icd1);icd1=NULL;}
if(cd1!=NULL) {free(cd1); cd1=NULL;}
if(ntr!=NULL){free(ntr); ntr=NULL;}
if(ntr_new!=NULL){free(ntr_new); ntr_new=NULL;}
if(trkpwr!=NULL){free(trkpwr); trkpwr=NULL;}
if(jdgtr!=NULL){free(jdgtr); jdgtr=NULL;}
if(nharm!=NULL){free(nharm); nharm=NULL;}
if(frtr_ifnew!=NULL){free(frtr_ifnew); frtr_ifnew=NULL;}
 if(frtr_jtnew!=NULL){free(frtr_jtnew); frtr_jtnew=NULL;}
if(irdie_new!=NULL){free(irdie_new); irdie_new=NULL;}
if(frtr_ifsub!=NULL){free(frtr_ifsub); frtr_ifsub=NULL;}
if(frtr_jtsub!=NULL){free(frtr_jtsub); frtr_jtsub=NULL;}
if(irdie_sub!=NULL) {free(irdie_sub);  irdie_sub=NULL;}
if(iord1!=NULL)     {free(iord1);      iord1=NULL;}
if(iord_new!=NULL)  {free(iord_new);   iord_new=NULL;}
if(ibuf0!=NULL)     {free(ibuf0);      ibuf0=NULL;}
if(fstframe0!=NULL){free(fstframe0);   fstframe0=NULL;}
if(lstframe0!=NULL){free(lstframe0);   lstframe0=NULL;}
if(fmean0!=NULL)   {free(fmean0);      fmean0=NULL;}
if(magmean0!=NULL) {free(magmean0);   magmean0=NULL;}
if(iordtemp!=NULL) {free(iordtemp);   iordtemp=NULL;}
if(iordtemp1!=NULL){free(iordtemp1);  iordtemp1=NULL;}
if(x!=NULL)        {free(x);  x=NULL;}
if(y!=NULL)        {free(y);  y=NULL;}
if(rbuf!=NULL)     {free(rbuf);rbuf=NULL;}
if(rpbuf!=NULL)    {free(rpbuf);rpbuf=NULL;}
if(iord!=NULL)    {free(iord); iord=NULL;}
if(irdie!=NULL)   {free(irdie); irdie=NULL;}
if(frtr!=NULL)    {free(frtr); frtr=NULL;}

/* if(arg!=NULL)freemem_c((char ***)&arg,MAXARG); */
/* main related buffers */
if(fn_input     !=NULL){free(fn_input); fn_input=NULL;}
if(fntmp     !=NULL) {free(fntmp); fntmp=NULL;}
}
/* --------------------------------------------
            sinued's error code
            coded by naotoshi Osaka
                   2/7/00
  --------------------------------------------- */
void sinuedErr(int ie)
{int ie0;
/* --------- */
ie0=ie * (-1);
switch(ie0){
 case 1:  /* opening file error */
    printf("            Check 1) existence, 2) owner, 3)'/'at the last \n");
  break;
 case 2:
    printf("sinued Er. -2: sinupkt not found. Errored filename=%s\n",fntmp);
  break;
 case 3:
   /*    printf("sinued Er. -3: ndata .GE. ndatamax.\n"); */
  break;
 case 4:
   printf("sinued Er. -4: nharm .GT. NENV. \n");
  break;
 case 5:
   printf("sinued Er. -5: ntrall .GT. NTRACK. But not used now. meaningless!\n");
  break;
 case 6:
   printf("sinued Er. -6: soundfile open error\n");
  break;
 case 7:
   printf("sinued Er. -7: harmonics file (.tjf) open error \n");
  break;
 case 8:
   printf("sinued Er. -8: magtrace open error\n");
  break;
 case 9:
   printf("sinued Er. -9: iord weird. \n");
  break;
  case 10:
   printf("sinued Er. -10: packet size exceeds ndata. \n");
   printf("                Sinupkt being used is wrong.\n");
  break;
 case 11:
   printf("sinued Er. -11: harmdata file (.tjt) for matlab open error \n");
  break;

}

}

