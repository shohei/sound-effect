#include <math.h>
void mhamwdw1(double wdw[],int N,int S) { // N: Window length, S: Frame shift length
    int i;
    double d,cons, phi, pi=3.14159265358979323846264338327950288419716939;
    cons=2.0*sqrt((double)S/(double)N) / sqrt(4*0.54*0.54 + 2*0.46*0.46);
    if(N>0) {
        d=2.0*pi/(double)(N);
        phi=pi/N;
        for(i=0; i<N; i++) wdw[i]= cons*(0.54-0.46*cos(d*i+phi));
    }
}
