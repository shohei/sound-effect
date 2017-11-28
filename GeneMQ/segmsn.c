#include <stdio.h>
#include <math.h>
/* -----------------------------------------------------
     segmsn
   Description:
     segmental S/N derivation for 1 frame
   Input: rorgbuf (double) original signal
          rbuf (double)    processed signal
          nsample (int)     Number of samples in 1 frame
   return value (double)   S/Nseg (db)
    
      coded by Naotoshi Osaka,     Oct/30/1993
   ----------------------------------------------------- */
double segmsn(double rorgbuf[],double rbuf[],int nsample)
{ double sn,temp,sumorg,sumnoise; int i;
  sumorg=sumnoise=0.0;
 for(i=0;i<nsample;i++)
 {sumorg += rorgbuf[i]*rorgbuf[i];
  sumnoise += (temp=(rorgbuf[i]-rbuf[i]))*temp;
  }
  if(sumnoise == 0.0) sn=300;
  else if(sumorg/sumnoise<=0.0)sn= -100.0;
   else  sn= 10.0*log10(sumorg/sumnoise); 
 return(sn);
}
