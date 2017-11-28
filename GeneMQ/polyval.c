/* ------------------------------------------------------------------------
    polyval
      y= polyval(p,x);
    p[]:  (double array)  coefficients of polynomial; from higher order
    x:    (double)  input x value.
    n:    (int)     dimension (number of coeef. is n+1. Be careful.
    return value: double y;
                Coded by Naotoshi Osaka, 6/25/1994
   ------------------------------------------------------------------------ */
#include <math.h>
double polyval(double p[],double x,int n) 
{ int i,j; double y;
 if(n==0){return(p[0]);}
 else if (n==1){return(p[0]*x + p[1]);}
 else if (n==2){return(p[0]*x*x+p[1]*x+p[2]);}
 else{
 y=0.0;
 for(i=0;i<=n-2;i++){y +=p[i]*pow(x,(double)(n-i));}
 y += p[n-1]*x + p[n];
 }
 return(y);
}
/* main()
{double y,x,p[5]; double polyval(); int i,j,n;
p[0]=2.0; p[1]=3.0; p[2]=4.0; p[3]=5.0; p[4]=6.0;
x=2;
for(j=0;j<5;j++){
printf("p[%d]=%lf ",j,p[j]);
}
printf(" \n");

n=0;
y=polyval(p,x,n);
printf("n=%d x=%lf y=%lf",n,x,y);
n=1;
y=polyval(p,x,n);
printf("n=%d x=%lf y=%lf",n,x,y);
n=2;
y=polyval(p,x,n);
printf("n=%d x=%lf y=%lf",n,x,y);
n=3;
y=polyval(p,x,n);
printf("n=%d x=%lf y=%lf",n,x,y);
n=4;
y=polyval(p,x,n);
printf("n=%d x=%lf y=%lf",n,x,y);
 
} */
