/* ------------------------------------------------------------------
    Library for signal processing
      wdwsgnlpv
    Description:
      prepare window data multiplied to signal into double buffer


  Input:
    rbuf: buffer (double) for signal processing
    wdw: window data (double)
    N: (int) =nfft; a unit
    L: (int) integer. Total wndow length = 2*L*N+1
    nth:(int)
  Output:
    wrbuf: (double) buffer size is 2*L*N+1, but effective output length
            is nfft.
    return value: none
                                         h(n)
                                          ^
                                         / \
                             /^\        /   \         /^\
                            /   \      /     \       /   \
                       ----+-----+----+---+---+-----+-----+-------> n
                         -3N -2N \   /-N  0  N \   /2N    3N
                                  \_/           \_/
   L=3's case

    Coded by Naotoshi Osaka        12/8/1994
   ------------------------------------------------------------------ */
#include <stdio.h>
#include <math.h>
void wdwsgnlpv(double rbuf[],double wdw[],int N,int L,
               double wrbuf[],int nth)
{   int i,j;
    double sum;
    for(i=0; i<2*L*N+1; i++) {
        wrbuf[i]=wdw[i]*rbuf[i];
        /*      printf(" wr0buf[%4d]=%lf wdw[%4d]=%lf rbuf[%4d]=%lf\n",
              i,wrbuf[i],i,wdw[i],i,rbuf[i]);  */
    }

    for(i=0; i<N; i++) {
        sum=0.0;
        for(j=0; j<2*L; j++) {
            sum += wrbuf[N*j+i];
        }
        wrbuf[i]=wrbuf[N+i]=sum;  /* Wrbuf[N+i] is used to preserve wrbuf[i] */
        /*     printf(" wrbuf[%4d]=%lf\n",i,wrbuf[i]); */
    }
    /* --------------- circular shift ---------------- */
    /*
      if(nth>0){
        for(i=0;i<nth;i++){wrbuf[i]=wrbuf[N-nth+i];}
        for(i=0;i<N-nth;i++){wrbuf[nth+i]=wrbuf[N+i];}
     }
      else if(nth<0){
        for(i=0;i<N+nth;i++){wrbuf[i]=wrbuf[-nth+i];}
        for(i=0;i< (-nth);i++){wrbuf[N+nth+i]=wrbuf[N+i];}
     } */
    /* ---  other direct method --- changing cicular shift into linear shift */
    /*  x <- x(m-nth) shift */
    if(nth>0) { /* right shift */
        for(i=0; i<N; i++) {
            wrbuf[i]=wrbuf[N-nth+i] ;
        }
    }
    else if(nth<0) { /* left shift */
        for(i=0; i<N; i++) {
            wrbuf[i]=wrbuf[-nth+i];
        }

    }
}
