/* -------------------------------------------------------
   fft
  Description:
    fft and inverse fft where variables are double.
      modified from Okuda's code.
      Decimation in time Fig 6.10 of 
      Oppenheim/Schafer, "Digital Signal Processing,"
      Prentice-Hall, Inc., 1975
      bit reverse order input, and normal order output:
      butterfly computation.
  Variables:
    input:                                     -1
      n:  number of fft point   in case n<0 fft   (inverse fft)
      x:  real data
      y:  imaginary data
    output:
      x:  real data
      y:  imaginary data
    return value:
      0: normal return -1: failure
    coded by Naotoshi Osaka, 8/9/1993
   ------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define  PI 3.14159265358979323846
int fft(int n,double x[],double y[])
{   static int last_n=0;
    static int *bitrev=NULL;
    static double *sintbl=NULL;
    int i,j,k,ik,h,d,k2,n4,inverse;
    int nn,le1,le,m,ip;
    double t,s,c,dx,dy;
    void make_sintbl(int n,double sintbl[]);
    void make_bitrev(int n,int bitrev[]);

    if(n<0) {
        n =-n;
        inverse=1;
    }
    else inverse=0;
    nn=n;
    n4=n/4;
    if(n!=last_n || n==0) {
        last_n=n;
        if(sintbl!=NULL) free(sintbl);
        if(bitrev !=NULL) free(bitrev);
        if(n==0) return 0;
        sintbl=malloc((n+n4)*sizeof(double));
        bitrev=malloc(n*sizeof(int));
        if(sintbl == NULL || bitrev == NULL) {
            fprintf(stderr,"not memory area enough");
            return 1;
        }
        make_sintbl(n,sintbl);
        make_bitrev(n,bitrev);
    }
    /* -------------- bit reverse ------------------ */
    for(i=0; i<n; i++) {
        j=bitrev[i];
        if(i<j) {
            t=x[i];
            x[i]=x[j];
            x[j]=t;
            t=y[i];
            y[i]=y[j];
            y[j]=t;
        }
    }
    /* -------------- calculation ------------- */
    le1=1;
    for(;;) {
        nn /=2;
        if(nn<1) break;
        le= le1 + le1;
        h=0;
        d=n/le;
        for(j=0; j<le1; j++) {
            c= sintbl[h+n4];
            s = sintbl[h];
            if(inverse) s = -sintbl[h];
            for(i=j; i<n; i +=le) {
                ip= i+le1;
                dx=  x[ip]*c + y[ip]*s;
                dy= -x[ip]*s + y[ip]*c;

                x[ip] = x[i] -dx;
                y[ip] = y[i]-dy;
                x[i] += dx;
                y[i] += dy;
            }
            h +=d;
        }
        le1=le;
    }
    /* -------- calculation end -------------- */

    /* if(! inverse) */
    if(  inverse)
        for(i=0; i<n; i++) {
            x[i] /= n;
            y[i] /= n;
        }
    return 0;
}
/* -------------------------------------------------------
   Make sine table
   ------------------------------------------------------- */
void make_sintbl(int n,double sintbl[])
{   int i,n2,n4,n8;

    double c,s,dc,ds,t;
    n2= n/2;
    n4 = n/4;
    n8 = n/8;
    t = sin(PI/n);
    dc = 2*t*t;
    ds= sqrt(dc*(2-dc));
    t= 2*dc;
    c=sintbl[n4]=1;
    s= sintbl[0]=0;
    for(i=1; i<n8; i++) {
        c -=dc;
        dc +=t*c;
        s += ds;
        ds -=t*s;
        sintbl[i]=s;
        sintbl[n4-i]=c;
    }
    if(n8 !=0)sintbl[n8]=sqrt(0.5);
    for(i=0; i<n4; i++) sintbl[n2-i]=sintbl[i];
    for(i=0; i<n2+n4; i++) sintbl[i+n2] = -sintbl[i];
}
/* -------------------------------------------------------
   bit reverse routine: 
   ------------------------------------------------------- */
void make_bitrev(int n,int bitrev[])
{   int i,j,k,n2;
    n2=n/2;
    i=j=0;
    for( ; ; ) {
        bitrev[i]=j;
        if(++i >=n) break;
        k=n2;
        while(k<=j) {
            j -= k;
            k /=2;
        }
        j += k;
    }
}
