/* -----------------------------------------------------------------------
   fpeakinwdw1:  < fpeakinwdw
 Difference: include file is different
 Description
   find peak magnitude given window in time and frequency domain.
                              -----          ¢å---------
                                    \        /
                                 +---\------/-------+
                              f  |    ------        |
                              w  |    --------------+--------
                              i  |                  |
                              n -+------------- (1) |
                              d  |                  |
                              o  |                  |
                              w  |  -----------     |
                                 +------------------+
                             (Hz)      twindow
 This is the very first version, and it lacks the universality.
 twindow is in frame. fwindow is in Hz. But Actural calculation here are
 all in terms of mean magnitude and frequency of tracks.
 Partial (1) want to be tested.
 For this version, twindow is almost meaningless. Even one point of the 
track is in the f and t window, the data is considered.

           Coded by Naotoshi Osaka   6/14/1995
                                    12/26/1998
   ----------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <math.h>
#define PAI 3.141592653589793238462643383

double fpeakinwdw1(int twindow,double fwindow,int ntr[],double fmean[],
   double magmean[],int fstframe[],int lstframe[],
   int nframe,int ntrall,int rst,int *peakpos)
{ double PI2,peak,fwdwedgeLow,fwdwedgeUp,twdwedgeR,twdwedgeL;
  int i,j,icont,ipeakpos,centerf;

  PI2=2.*PAI; peak=0.0;
  centerf=(fstframe[rst]+lstframe[rst])/2;
  if((fwdwedgeLow= fmean[rst]-fwindow)<0) fwdwedgeLow=0.;
  fwdwedgeUp=fmean[rst]+fwindow;
  if((twdwedgeL=centerf-twindow)<0)twdwedgeL=0;
  if((twdwedgeR=centerf+twindow)>nframe)twdwedgeR=nframe;
  for(i=icont=0;i<ntrall;i++){
   if(fmean[i]> fwdwedgeLow && fmean[i]<fwdwedgeUp){
     icont=1;
     if(fstframe[i]<twdwedgeR && lstframe[i] > twdwedgeL){
       if(magmean[i]>peak){peak=magmean[i];ipeakpos=i;}
     }
   }
   else if(icont==1){break;}
  }
 *peakpos= ipeakpos;
  return(peak);
}
