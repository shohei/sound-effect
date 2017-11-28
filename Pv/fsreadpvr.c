/* ------------------------------------------------------------------
    Library for signal processing
      fsreadpvr
    Description:
      File I/O (1)
    to prepare filtering-necessary signal data to buffer while reading signal
    data with frame length shift.
     <-------- Buffer length   ------->
     =============+=====================
     |<--nshift-->|<-- nsize-shift   -->|
                       /
                      V
     =====================++++++++++++++    +: read new data from file
    sbuf: buffer (short) for reading signal file
    rbuf: buffer (double) for signal processing
    smsec:(double) frame shift in msec
    sf   :(double) sampling frequency in kHz
    nfft: (int) number of fft point
    L:    (int) filter length relevant integer. [nfilter = 2*L*nfft+1]
    nleft:(* int)number of left process in buffer; necessary
                 when index == -1
    input: signal file stream
    index: 0: initiallize and pad zero for the first half
           1: normal process;  action drawn above
          -1: final process; do not read file any more and proceed
                              on buffer only.
    return value: 1: normal return
                 -1: will enter the final process, that is, the next
                     time the function is called, index should be -1
                     to do the final process.
                 -2: the real end of the data reading process
    caution: sbuf is normally (when index==1) used in nshift length.
             but in initiallization (when index==0) it should be in L*nfft+1.
             Therefore, you can use other buffer for initiallization, or
             prepare nfft-length buffer for sbuf.
   * the difference from fsreadpv :
     1) smsec is real.


    Coded by Naotoshi Osaka        1/9/1995
   ------------------------------------------------------------------ */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
int fsreadpvr(short sbuf[],double rbuf[],double smsec,double sf,
              int nfft,int *nleft,int L,int Q,FILE *input,int index)
{   int i,ie,nshift,rvalue,ntotal,ndata,R,nsize;
    R=nshift=(int)(smsec*sf);   /* R and nshift are equivalent */
    nsize= 2*L*nfft+1;
    if(index==0) { /* initiallize */
        /* Be careful. buffer size here is nfft */
        ndata=L*nfft+1-(Q-1)*R;
        if(ndata<=0) {
            printf("fsreadpv error. Change Q/R to let ndata>0.\n");
            exit(0);
        }
        ie=fread((char *)sbuf,sizeof(short),ndata,input);
        if(ie != ndata) {
            for(i=0; i<L*nfft+(Q-1)*R; i++) {
                rbuf[i]=0.0;
            }
            for(i=0; i<ie; i++) {
                rbuf[i+L*nfft+(Q-1)*R]=sbuf[i];
            }
            rvalue= -1;
            ntotal= ie;
        }
        else {
            for(i=0; i<L*nfft+(Q-1)*R; i++) {
                rbuf[i]=0.0;
            }
            for(i=0; i<ndata; i++) {
                rbuf[i+L*nfft+(Q-1)*R]=sbuf[i];
            }
            rvalue=1;
        }
    }

    else if (index==1) /* normal process */
    {   ie=fread((char *)sbuf,sizeof(short),nshift,input);
        if(ie!=nshift) {
            for(i=0; i<nshift-ie; i++) {
                sbuf[i+ie]= 0;
            }
            rvalue= -1;
            ntotal= nsize - nshift + ie;
        }
        else {
            rvalue=1;
        }
        for(i=0; i<nsize -nshift; i++) {
            rbuf[i]=rbuf[i+nshift];
        }
        for(i=0; i<nshift; i++) {
            rbuf[i+nsize-nshift]=sbuf[i];
        }
    }

    else if(index== -1)  /* final shori */ /* ntotal: effective data length */
    {   nshift=(int)(smsec*sf);
        ntotal= *nleft;
        for(i=0; i<ntotal-nshift; i++) {
            rbuf[i]=rbuf[i+nshift];
        }
        for(i= (ntotal-nshift); i< nsize; i++) {
            rbuf[i]=0.0;
        }
        if(ntotal-nshift> (L*nfft-Q*nshift)) {
            ntotal -= nshift;
            rvalue= -1;
        }
        else if(ntotal-nshift==(L*nfft-Q*nshift)) {
            rvalue= -2;
        }
        else {  /*  0 < ntotal-nshift < (L*nfft-Q*nshift) */
            for(i=0; i< (L*nfft-Q*nshift)+nshift-ntotal; i++) {
                rbuf[i+ntotal-nshift]=0.0;
            }
            rvalue = -2;
        }
    }  /* end of final shori */
    if(rvalue < 0)*nleft=ntotal;
    return(rvalue);
}
/* --------------------------------------------------- */
/* #include <stdio.h>
#include <math.h>
#define NFFTP1 21
main()
{
short sbuf[5],sbbuf[123]; double rbuf[NFFTP1],smsec;
int nfft,index,i,j,ie; double sf;
FILE *input; int fsread();

nfft=NFFTP1-1;smsec=5.0; sf=1.0;

for(i=0;i<123;i++)sbbuf[i]=i+1;
input=fopen("testbuf","w");
fwrite((char *)sbbuf,123*2,1,input);
fclose(input);
input=fopen("testbuf","r");
printf("index=0\n");
  ie= fsreadpvr(sbuf,rbuf,smsec,sf,nfft,input,0);
for(i=1;i<30;i++){
  ie= fsreadpvr(sbuf,rbuf,smsec,sf,nfft,input,i);
  printf("i=%d\n  ",i);
  for(j=0;j<nfft+1;j++)printf("%6.0lf",rbuf[j]);
  printf("\n");
  if(ie<0){break;}
 }
  printf("i=%d\n  ",i);
  for(j=0;j<nfft+1;j++)printf("%6.0lf",rbuf[j]);
  printf("\n");
fclose(input);
}  */
