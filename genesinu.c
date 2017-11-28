void gensinu(struct trackpkt *tbuf, double *a_Theta0, 
             double *a_Theta1, double *a_Omega0, 
             double *a_Omega1, double *a_Mag0, double *a_Mag1, 
             double ssec, double PI2, double sf, 
             double rbuf[NFFTP1], double rpbuf[], int nshift,
             int nfft, double *delta, int *lastj1, int iframe,
             int *idlast, double *insferr0, int nwdw, 
             int iroute, FILE *output2, int index, int idw)

{
  double PI,x,M,miu1,matrixup,matrixdown,t;
  double Alpha,Beta,theta,delta1,delta0;
  double Theta0,Theta1,Omega0,Omega1,Mag0,Mag1;
  int n,j;
  static double THETA0 = 0.0,tsec;

  double deltaf,deltafrate,foft,insferr;
  double tempdata;
  int nbias,ie;

  Theta0 = *a_Theta0;
  Theta1 = *a_Theta1;
  Omega0 = *a_Omega0;
  Omega1 = *a_Omega1;
  Mag0 = *a_Mag0;
  Mag1 = *a_Mag1;
  PI = PI2/2.0;
  tsec = (double)nfft/(sf*1000.0);   /* N/(sf*1000) */

label1:
  x = ((Theta0 + Omega0*ssec-Theta1)
       +(Omega1-Omega0)*ssec/2.0)/(PI2);
  M = x-(M=(double)((int)x)) < M+1.0-x ? M :M+1.0 ;
  miu1 = M-x;
  /* ----------------------------------- */
  deltaf = (Omega1-Omega0)/PI2;
  deltafrate = deltaf/(sf*1000./(double)nfft);

  /* ----- End of modified process ----------------- */
  if(index==1) {
    for(n=0; n<nshift; n++) { /* -- loop for each sample -- */
      t = (double)n/(sf*1000.0);
      theta = Theta0+Omega0*t;
      if(n==0 || n==nshift-1) {
      }
      rbuf[n] += (Mag0+(Mag1-Mag0)*(double)n/(double)nshift)
                 * cos(fmod(theta,PI2));
      rpbuf[n] = theta;
    }                     /* -- loop end for each sample -- */
    insferr = 0.0;
    foft = Omega0;
  } else {
    matrixup = Theta1-Theta0-Omega0*ssec+M*PI2;
    matrixdown = Omega1-Omega0;
    Alpha = matrixup*(3.0/(ssec*ssec))-matrixdown/ssec;
    Beta = matrixup*(-2.0/(ssec*ssec*ssec))
           +matrixdown/(ssec*ssec);
    insferr = 0.;
    nbias = (nwdw/2*2==nwdw ? nwdw/2 : (nwdw-1)/2);
    for(n=0; n<nshift; n++) { /* -- loop for each sample -- */
      t = (double)n/(sf*1000.0);
      theta = Theta0+Omega0*t+Alpha*t*t+Beta*t*t*t;
      foft = Omega0+2.0*Alpha*t+3.0*Beta*t*t;
      rbuf[n] += (Mag0+(Mag1-Mag0)*(double)n/(double)nshift)
                 * cos(fmod(theta,PI2));
      rpbuf[n] = theta;
      /* insf evaluation */
      insferr += 0.;
      /*   (foft-insf[nbias+iframe*nshift+n])
           *(foft-insf[nbias+iframe*nshift+n]); */
      /*  printf("gen: foft[%d]=%lf insf[%d]=%lf (Hz)\n",
             n,foft/PI2,n,insf[nbias+iframe*nshift+n]/PI2); */
    }                     /* -- loop end for each sample -- */
  }
  insferr = sqrt(insferr/(double)nshift)/PI2;
  *insferr0 = insferr;
  /* If below is changed, change npk, and sinupktNew.h */
  tbuf->nTheta0 = (int)(Theta0*pow(2.,16.));
  tbuf->nTheta1 = (int)(Theta1*pow(2.,16.));
  tbuf->nOmega0 = (int)(Omega0*pow(2.,8.));
  tbuf->nOmega1 = (int)(Omega1*pow(2.,8.));
  tbuf->Mag0 = (short)Mag0;
  tbuf->Mag1 = (short)Mag1;
  tbuf->iroute = (short)iroute;
}
