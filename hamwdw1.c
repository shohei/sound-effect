#include <math.h>
void hamwdw1(double wdw[],int n) {
    int i;
    double d, pi=3.14159265358979323846264338327950288419716939;
    if(n>0) {
        d=2.0*pi/(double)(n-1);
        for(i=0; i<n; i++) wdw[i]=0.54-0.46*cos(d*i);
        wdw[n-1]=wdw[0]=0.54-0.46;
    }
}
