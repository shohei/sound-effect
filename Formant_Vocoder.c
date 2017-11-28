#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define NBUF1 2000
#include "wavlib.h"
int main()
{
  void resfil(double x[], double y[], double v[], double a,
              double w, double T, int n, int id, double E);
  int i,j,jj,k,nsample,nsize,nres,fmt_id;
  int tau1,tau2;
  int index,id,idg_wave;
  int nrepeat,ipos;
  wav_head bufhead;
  short *xx;
  double *x,**y,**y1;
  double F[5][10]; /* F[0][*]: pitch */
  double pi,twopi,expo,fs,fk,sec,a[4],w,dt,band[4];
  double ratio,morapsec,gain,f0,a1,rindex;
  char *namesigout="sig.wav";
  /* ------------------------------------------------------ */
  twopi = 2.0*(pi=3.14159265358979323846);
  expo = 2.7182818284590452354;
  fs = 22050;   /* sampling frequency in Hz */
  sec = 1.6;    /* speech wave duration (sec) */
  idg_wave = 1; /* glottal wave identifier; 
                   id: 0/1= Triangular/impulse */
  dt = 1./fs;
  nsample = (int)(fs*sec);
  morapsec = 10.0/3.0; /* utterance speed */
  gain = 0.50;
  nres = 4;
  fmt_id = 1;
  xx = bufhead.data = calloc(nsample,sizeof(short));
  x  = calloc(NBUF1,sizeof(double));
  y  = calloc(nres,sizeof(double *));
  y1 = calloc(nres,sizeof(double *));
  for(j=0;j<=nres;j++){
    y[j]  = calloc(NBUF1,sizeof(double));
    y1[j] = calloc(NBUF1,sizeof(double));
  } 
  /* -----------------------------------------------------
      F0 and Formant Frequency definition
     ----------------------------------------------------- */
  F[0][0]=110.;F[1][0]=250.;F[2][0]=2100.;
    F[3][0]=2900.; F[4][0]=3700.;
  F[0][1]=160.;F[1][1]=450.;F[2][1]=1900.; 
    F[3][1]=2650.; F[4][1]=3800.;
  F[0][2]=110.;F[1][2]=700.;F[2][2]=1250.; 
    F[3][2]=2500.; F[4][2]=3900.;
  F[0][3]=110.;F[1][3]=500.;F[2][3]=1050.; 
    F[3][3]=2700.; F[4][3]=3700.;
  F[0][4]=110.;F[1][4]=330.;F[2][4]=1500.; 
    F[3][4]=2400.; F[4][4]=3650.;
  F[0][5]=110.;F[1][5]=335.;F[2][5]=1550.; 
    F[3][5]=2450.; F[4][5]=3800.;
  f0 = (F[0][0]+F[0][1])/2.0;
  band[0] = 50.0; band[1]=70.0; band[2]=110.0; band[3]=200.0;
  for(j=0;j<4;j++) a[j]=pi*band[j];
  gain *= 0.5;
  /* ---------------------- Big loop ---------------------- */
  for(i=ipos=nrepeat=id=0;; i++) {               
    if(ipos>nsample) break;
    rindex = dt*morapsec*(double)ipos;
    index = (int)rindex;
    if(index>4) index=4;
    /* pitch frequency decision  */
    nrepeat = fs/(f0=(F[0][0]+F[0][1])/2.0
                  +(F[0][0]-F[0][1])/2.0
                  *sin(twopi*dt*morapsec/5.*(double)ipos
                       +twopi/4.));
    /* ----------------------------------------------------
       Triangular approximation to glottal wave
       ---------------------------------------------------- */
    if(idg_wave==0) {
      tau1 = 0.25*(double)nrepeat;
      tau2 = 0.50*(double)nrepeat;
      for(j=0; j<nrepeat; j++) {
        if(j<=tau1)
          x[j] = gain*(double)j/(double)tau1;
        else if (j<tau1+tau2)
          x[j] = -gain/tau2*(double)(j-tau1)+gain;
        else x[j] = 0.0;
      }
    }
    /* ----------------------------------------------------
       Impulse approximation to glottal wave
       ---------------------------------------------------- */
    else {
      x[0] = gain;
      for(j=1; j<nrepeat; j++) x[j]=0.0;
    }
    /* ---------------------------------------------------- */
    for(k=1; k<=4; k++) { /* Parallel process */
      fk = F[k][index]+(F[k][index+1]-F[k][index])
           *(rindex-(double)index);
      w = twopi*fk;
      resfil(x,y[k-1],y1[k-1],a[k-1],w,dt,nrepeat,id,expo);
      if(k!=1) {
        for(j=0; j<nrepeat; j++) y[0][j] += y[k-1][j];
      }
    }
    for(j=0; j<nrepeat; j++) xx[ipos+j] = y[0][j];
    ipos = ipos+nrepeat;
    id=1;
  }
  /* ---------------- End of the big loop ----------------- */
  wavwrite(&bufhead,1,(long)fs,bits,nsample,namesigout,fmt_id);

  free(xx);
  free(x);
  free(y);
  free(y1);
  return 0;
}

/* ======================================================== */
void resfil(double x[], double y[], double v[], double a,
            double w, double T, int n, int id, double E)
{
  int i,j;
  double B,C,D;

  B = 2.0*pow(E, -a*T)*cos(w*T);
  C = pow(E, -2.0*a*T);
  D = ((a*a+w*w)/w)*pow(E, -a*T)*sin(w*T);
  if(id==0) v[0] = v[1] = 0.0;
  else {
    v[0] = v[n-2];
    v[1] = v[n-1];
  }
  for(i=0; i<n; i++) {
    v[i+2] = B*v[i+1]-C*v[i]+x[i];
    y[i] = D*v[i+1];
  }
}
