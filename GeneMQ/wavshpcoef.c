/* ------------------------------------------------------------------
    --- wavshpcoef ---
     Signal processing function library       #          
               coded by N.Osaka,                  06/25/1994    

   description:
          -----------------------
   wavc[][]    : output; coefficients of Chebychev polinomials 
                 of the first kind.
                 wavc[i][j] represents coeffcientof x**j in Ti(x).
                 Approximately 3/4 of the buffer is 0.
   n       : input.  integer. in Tn(x).
   return value: none.
  ------------------------------------------------------------------- */
#include <stdio.h>
#include <math.h>
int wavshpcoef(double wavc[31][31],int n,double a)

{  int i,j,ie; 
/* ---------- clear coef. buffer ---------- */
  if(n>30){printf("Error (wavshpcoef) n > nmax(=30). change pro.\n");
   return(-1);}
  if(a != 1.0){printf("Error (wavshpcoef) a should be 1.0 (double) now. \n");
   return(-2);}
/* ---------- clear coef. buffer ---------- */
  for(i=0;i<n;i++){for(j=0;j<n;j++){wavc[i][j]=0.0; }}
/* ------------------------------------- */
 wavc[0][0]=wavc[1][0]=1.0;
 if(n<=1)return(0);
 else{
  for(i=2;i<=n;i++){
   wavc[i][0]=wavc[i-1][0]*2.0;
   for(j=2;j<i;j++){
     wavc[i][j] = wavc[i-1][j]*2.0 - wavc[i-2][j-2];
   }
   wavc[i][i]= -wavc[i-2][i-2];
  }
 }
return(0);
}
/* #include <stdio.h>
#include <math.h>
main()
{
  int i,j,nmax,n;
static double wavc[31][31];
  nmax=30;
  n=12;
  wavshpcoef(wavc,n,1.0);
  for(i=0;i<=n;i++){
   printf(" \n n=%d",i);
   for(j=0;j<=i;j++){
    printf("%6.0lf ",wavc[i][j]);
   }
  }
} 
*/
