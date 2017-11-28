#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

int cmd_pkedit(int *icf,int *icf1,int *icm,int *icm1,int *icd,int *icd1,double *cd1)
{
int tprob,comcl;

/* ---------------packet edit ---------------------- */
func1:
 printf(" TR# and command? \n");
 printf("  (command:  0/1/2/3=freq/mag/delay/del)\n");
 printf("   (TR#:  -1 0 (0=dummy): exit packet edit. \n");
 scanf("%d %d",&tprob,&comcl);
 if(tprob<0){goto labelbase2;}

 switch(comcl){
 case 0:
   printf(" operation: 0:org 1:f0 2:ref 3:model sin 4:model ch 5:model fm\n");
   printf("            6:concat 7:INT 8:del 9:rep\n");
   scanf("%d",&icf[tprob]);
   if(icf[tprob]==2){printf(" reference tr#?"); scanf("%d",&icf1[tprob]);}
   if(icf[tprob]==6){printf(" concatinate tr#?"); scanf("%d",&icf1[tprob]);}
   if(icf[tprob]==7){
    /* printf(" interpolation tr#?"); scanf("%d",&icf1[tprob]); */
     icf1[tprob]=tprob;
     }
   if(icf[tprob]==9){printf("  tr#?"); scanf("%d",&icf1[tprob]);}
   break;
 case 1:
   printf(" operation: 0:org 1:f0 2:ref 3: f0pt 4:ref pt \n");
   printf("            5:model lin 6:model am 7:mean\n");
   scanf("%d",&icm[tprob]);
   if(icm[tprob]==2){printf(" reference tr#?"); scanf("%d",&icm1[tprob]);}
   if(icm[tprob]==4){printf(" reference tr#?"); scanf("%d",&icm1[tprob]);}
   if(icm[tprob]==5){printf(" reference tr# to mean power?");
                     scanf("%d",&icm1[tprob]);}
   if(icm[tprob]==6){printf(" reference tr# to mean power?");
                     scanf("%d",&icm1[tprob]);}
   if(icm[tprob]==7){printf(" reference tr# to mean power?");
                     scanf("%d",&icm1[tprob]);}
   break;
 case 2:
   printf(" operation: 0:org 1:fr#  2:ph(rad) \n");
   scanf("%d",&icd[tprob]);
   if(icd[tprob]==1){printf(" delay in frame count (int)?");
           scanf("%d",&icd1[tprob]);}
   if(icd[tprob]==2){printf(" delay in radian (double)?");
           scanf("%lf",&cd1[tprob]);}
   break;
 case 3:
   icf[tprob]=8; /* delete */
 }
 goto func1;

labelbase2:
return(0);

}

