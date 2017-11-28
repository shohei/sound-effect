/* ------------------------------------------------------------
 --- lpfdsn1.c ---
     General FIR filter disign : to find impulse response
     of ideal low pass filter (For phase vocoder)
             sin(n(J/N)
      h(n)= -----------
               nJ/N
      n= -L*N, ... 0 , ... L*N
      Total number of n: 2*L*N+1

      output: wdw[]*h[]

Caution:
      Output buffer starts in convenience from 0,1,... (2LN+1)-1.
      But actually it represents n, that is, -L*N,-L*N+1,...,0,...,L*N
      Keep this in mind. The difference is 0 phase or not.

               Coded by N.Osaka      12/2/1994
 -------------------------------------------------------------- */
#include <math.h>
void lpfdsn1(int N,int L,double wdw[],double f[])
{   int i;
    double d,x,pi=3.14159265358979323846;
    double alp;
    /* --------------------------------------------- */
    if(N>0)
    {   d= pi/(double)N;
        alp=(double)(L*N);
        for(i=0; i<L*N; i++) {
            /* -------------- first half of the calculation --------------- */
            f[i]=wdw[i]*sin(x=d*((double)i-alp))/x;
        }
        /* -------------- last half  of the calculation --------------- */
        for(i=L*N+1; i<2*L*N; i++) {
            f[i]=wdw[i]*sin(x=d*((double)i-alp))/x;
        }
        f[L*N]=wdw[L*N];
        f[2*L*N]=0.0;
    }
}
/* -------------------------------------------------------------------- */
/* #include <math.h>
#include <stdio.h>
#define NFILTER 251

int main(void)
{double fc,fsi,ham1[NFILTER],f[NFILTER];
 int i,nfilter,N,L;
 printf(" L, N?\n");
 scanf(" %d %d",&L,&N);
 nfilter=2*L*N+1;
 hamwdw1(ham1,nfilter);
 lpfdsn1(N,L,ham1,f);
 for(i=0;i<nfilter;i++)printf("ham1[%d]=%lf filter[%d]=%lf\n",
                       i,ham1[i],i,f[i]);
 return(0);
} */

