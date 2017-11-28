#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "wavlib.h"

int main(int argc, char *argv[])
{
  int fmt_id,Nmaterial,n,j,k,l,id;
  short *ibuf;
  char  *fn,*fncommon;
  int fs,Np,Nres,irandmax;
  double xb[7],xb0[7] = {2.0e-5, 5.0e-3, 2.0e-2, 5.0e-2, 0.1, 
                         0.2, 0.35 };
  double yb[7][3] = {0.89, 0.81, 0.65, 0.62, 0.45, 0.15, 
                     -10000, -10000, 0.05, -10000, 0.15, 
                     -10000, 0.15, 0.06, 0.0, 0.065, -10000, 
                     -10000, 0.02, 0.005, -10000};
  double xa[5],xa0[5] = { 2.0e-5,  5.0e-3,  4.0e-2,  0.1, 0.3};
  double ya[5][3] = {0.77, 0.62, 0.3, 0.3, -0.05, -0.7, -0.2, 
                     -0.53, -0.87, -0.42, -0.68, -10000., 
                     -0.59, -0.78, -0.92
                   };
  double b0[3],a1[3],a2[3],b2; // 1,2,3 correspond to Steel, 
                               // Glass and Wood, respectively.
  double t,tt; // analogue time and its log
  double A[4][4],Ainv[4][4],B[4],C[3];
  double *e,**yda,**ydb,*xd;
  double **s,*ybuf,**y,*yres;
  double a1th1,a1th2,a1th3;
  double sec; // 出力音の長さ〔s〕
  double Amp,maxAmp,Ampmax,Ampmin; // 相対振幅(0-1)と最大振幅
  double f0[10]; // = {136.,166.,465.,571.,700.,1500.};
  double Q[10];  // = {200.,200.,200.,200.,200.,200.};
  int ds[10];    // = {70,57,13,38,25,45};
  double temp1,temp2;
  char material[3][10] = {"Steel.wav","Glass.wav","Wood.wav"};

  wav_head bufhead;
  int resonant_IIR2nd_design(double f0, double Q, double Fs,
                             double *a1, double *a2);
  int resonant_IIR_delay(double e[], double a1, double a2, 
                         double b2, int ds, int Np, 
                         double ybuf[], double y[]);
  extern int inv3(double a[4][4],double ainv[4][4]);
  extern int Timbre_Paraset(double f0[],double Q[],int ds[],
                            double *b2,int id);
  if(argc != 3) {
    printf("\nusage: PhM_Armk filename id\n");
    printf("   filename       : Output sound file name ");
    printf("(,wav unnecessary)\n");
    printf("   id (=timbre id): 0: xylophone, 1:Steel Drum, ");
    printf("2: Wood 3: Steel  (like piano)\n");
    printf("                    4: Metal + Metal, 5:Metal + ");
    printf("Metal, 6:Wood and drums 7: piano sound\n");
    printf("                    8:good piano sound \n");
    return 0;
  }
  fn = calloc(100,sizeof(char));
  fncommon = calloc(100,sizeof(char));
  sscanf(argv[1],"%s",fncommon);
  id = atoi(argv[2]); // Timbre id
  irandmax = RAND_MAX;

  // fs = 44100;  // サンプリング周波数〔Hz〕
  fs = 48000;
  sec = 1.2;      // 音声の総合的な時間長〔s〕
  Np = (int)((double)fs*sec); // 総合サンプル数
  fmt_id = 1;     // 出力ファイル書式：
                  // 1: wav file, 0: Raw file format
  Nmaterial = 3;  // 素材数
  // Amp = 0.1;      // 相対振幅
  maxAmp = 32768.*pow(10.,-3.01/20.);  // 16bit shortの最大値よ
                                       // り3dB低い値 = 23171
  /* ------------------------------------------------------
     メモリ確保　s[Nmaterial][Np]は2次元
     ------------------------------------------------------ */
  s = calloc(Nmaterial,sizeof(double *));
  for (j=0; j<Nmaterial; j++) s[j] = calloc(Np,sizeof(double));
  ybuf = calloc(Np,sizeof(double));
  y = calloc(Nmaterial,sizeof(double *));
  for (j=0; j<Nmaterial; j++) {
    y[j] = calloc(Np,sizeof(double));
  }
  e = calloc(Np,sizeof(double));
  ibuf = bufhead.data = calloc(Np,sizeof(short));
  xd = calloc(Np,sizeof(double));
  yda = calloc(Nmaterial,sizeof(double *));
  ydb = calloc(Nmaterial,sizeof(double *));
  for (j=0; j<Nmaterial; j++) {
    yda[j] = calloc(Np,sizeof(double));
    ydb[j] = calloc(Np,sizeof(double));;
  }
  for (j=0; j<7; j++) {
    xb[j] = log10(xb0[j]);
    if(j<5)xa[j] = log10(xa0[j]);
  }
  yres = calloc(Np,sizeof(double));

// 素材部　合成
// ランダム信号の合成     % e=a*(rand(1,Np)-0.5);
  srand((unsigned)time(NULL));
  for (n=0; n<Np; n++) {
    e[n] = ((double)rand()/(irandmax+1.)-0.5)*maxAmp;
  }
// 時変フィルタの設計 ydb[j][n], yda[j][n] の算出
  for (n=0; n<Np; n++) { // 離散時間のnのループ
    t = (double)(n+1)/(double)fs;
    tt = log10(t);
    xd[n] = tt;
// Steel
    if (t>0 && t<xb0[1])
      b0[0] = (yb[1][0]-yb[0][0])/(xb[1]-xb[0])
              *(tt-xb[0])+yb[0][0];
    else if (t>=xb0[1] && t<xb0[4])
      b0[0] = (yb[4][0]-yb[1][0])/(xb[4]-xb[1])
              *(tt-xb[1])+yb[1][0];
    else if (t>=xb0[4] ) {   // && t<xb0[6]
      for (j=1; j<=3; j++) { // 公式にあわせるため，A-Cは1はじ
                             // まりのバッファとして使用．0番目
                             // は未使用
        A[j][1] = xb[3+j]*xb[3+j];
        A[j][2] = xb[3+j];
        A[j][3] = 1.;
      }
      B[1] = yb[4][0];
      B[2] = yb[5][0];
      B[3] = yb[6][0];
      inv3(A,Ainv);
      for (j=1; j<=3; j++)C[j-1] = 
        Ainv[j][1]*B[1]+Ainv[j][2]*B[2]+Ainv[j][3]*B[3];
      b0[0] = C[0]*tt*tt + C[1]*tt + C[2];
      if (tt> (-C[1])/(2*C[0]) || b0[0] <0) b0[0] = 0;
    } else  b0[0] = 0;

// Glass
    if (t>0 && t<xb0[1])
      b0[1] = (yb[1][1]-yb[0][1])/(xb[1]-xb[0])
              *(tt-xb[0])+yb[0][1];
    else if (t>=xb0[1] && t<xb0[3])
      b0[1] = (yb[3][1]-yb[1][1])/(xb[3]-xb[1])
              *(tt-xb[1])+yb[1][1];
    else if (t>=xb0[3] ) {   // && t<xb0[6]
      for (j=1; j<=2; j++) { // 公式にあわせるため，A-Cは1はじ
                             // まりのバッファとして使用．0番目
                             // は未使用
        A[j][1] = xb[2+j]*xb[2+j];
        A[j][2] = xb[2+j];
        A[j][3] = 1.;
      }
      A[3][1] = xb[6]*xb[6];
      A[3][2] = xb[6];
      A[3][3] = 1.;
      B[1] = yb[3][1];
      B[2] = yb[4][1];
      B[3] = yb[6][1];
      inv3(A,Ainv);
      for (j=1; j<=3; j++)C[j-1] = 
        Ainv[j][1]*B[1]+Ainv[j][2]*B[2]+Ainv[j][3]*B[3];
      b0[1] = C[0]*tt*tt + C[1]*tt + C[2];
      if (tt> (-C[1])/(2*C[0]) || b0[1] <0) b0[1] = 0;
    }

// Wood
    if (t>0 && t<xb0[1])
      b0[2] = (yb[1][2]-yb[0][2])/(xb[1]-xb[0])
              *(tt-xb[0])+yb[0][2];
    else if (t>=xb0[1] ) {   // && t<xb0[2]
      for (j=1; j<=2; j++) { // 公式にあわせるため，A-Cは1はじ
                             // まりのバッファとして使用．0番目
                             // は未使用
        A[j][1] = xb[j]*xb[j];
        A[j][2] = xb[j];
        A[j][3] = 1.;
      }
      A[3][1] = xb[4]*xb[4];
      A[3][2] = xb[4];
      A[3][3] = 1.;
      B[1] = yb[1][2];
      B[2] = yb[2][2];
      B[3] = yb[4][2];
      inv3(A,Ainv);
      for (j=1; j<=3; j++)C[j-1] = 
        Ainv[j][1]*B[1]+Ainv[j][2]*B[2]+Ainv[j][3]*B[3];
      b0[2] = C[0]*tt*tt + C[1]*tt + C[2];
      if (tt> (-C[1])/(2*C[0]) || b0[2] <0) b0[2] = 0;
    }
    for (j=0; j<Nmaterial; j++) ydb[j][n] = b0[j];

// a1の導出
// Steel
    a1th1 = -0.65;
    if (t>0 && t<xa0[1])
      a1[0] = (ya[1][0]-ya[0][0])/(xa[1]-xa[0])
              *(tt-xa[0])+ya[0][0];
    else if (t>=xa0[1]) { // && t<xa0[4]
      A[1][1] = xa[1]*xa[1];
      A[1][2] = xa[1];
      A[1][3] = 1.0;
      A[2][1] = xa[3]*xa[3];
      A[2][2] = xa[3];
      A[2][3] = 1;
      A[3][1] = xa[4]*xa[4];
      A[3][2] = xa[4];
      A[3][3] = 1;
      B[1] = ya[1][0];
      B[2] = ya[3][0];
      B[3] = ya[4][0];
      inv3(A,Ainv);
      for (j=1; j<=3; j++)C[j-1] = 
        Ainv[j][1]*B[1]+Ainv[j][2]*B[2]+Ainv[j][3]*B[3];
      a1[0] = C[0]*tt*tt + C[1]*tt + C[2];
      if (tt> (-C[1])/(2*C[0]) || a1[0] <a1th1) a1[0] = a1th1;
    } else a1[0] = a1th1;

// Glass
    a1th2 = -0.8;
    if (t>0 && t<xa0[1])
      a1[1] = (ya[1][1]-ya[0][1])/(xa[1]-xa[0])
              *(tt-xa[0])+ya[0][1];
    else if (t>=xa0[1] && t<xa0[2])
      a1[1] = (ya[2][1]-ya[1][1])/(xa[2]-xa[1])
              *(tt-xa[1])+ya[1][1];
    else if (t>=xa0[2]) { // && t<xa0[4]
      for (j=1; j<=3; j++) {
        A[j][1] = xa[1+j]*xa[1+j];
        A[j][2] = xa[1+j];
        A[j][3] = 1.0;
      }
      B[1] = ya[2][1];
      B[2] = ya[3][1];
      B[3] = ya[4][1];
      inv3(A,Ainv);
      for (j=1; j<=3; j++)C[j-1] = 
        Ainv[j][1]*B[1]+Ainv[j][2]*B[2]+Ainv[j][3]*B[3];
      a1[1] = C[0]*tt*tt + C[1]*tt + C[2];
      if (tt> (-C[1])/(2*C[0]) || a1[1] <a1th2) a1[1] = a1th2;
    } else a1[1] = a1th2;
    
// Wood
    a1th3 = -0.92;
    if (t>0 && t<xa0[1])
      a1[2] = (ya[1][2]-ya[0][2])/(xa[1]-xa[0])
              *(tt-xa[0])+ya[0][2];
    else if (t>=xa0[1]) {// && t<xa0[4]
      for (j=1; j<=2; j++) {
        A[j][1] = xa[j]*xa[j];
        A[j][2] = xa[j];
        A[j][3] = 1.0;
      }
      A[3][1] = xa[4]*xa[4];
      A[3][2] = xa[4];
      A[3][3] = 1;
      B[1] = ya[1][2];
      B[2] = ya[2][2];
      B[3] = ya[4][2];
      inv3(A,Ainv);
      for (j=1; j<=3; j++)C[j-1] = 
        Ainv[j][1]*B[1]+Ainv[j][2]*B[2]+Ainv[j][3]*B[3];
      a1[2] = C[0]*tt*tt + C[1]*tt + C[2];
      if (tt> (-C[1])/(2*C[0]) || a1[2] <a1th3) a1[2] = a1th3;
    } else  a1[2] = a1th3;
    for (j=0; j<Nmaterial; j++) yda[j][n] = a1[j];
  } // 離散時間のnのループ終了

  for (n=0; n<Np; n++) xd[n] = (double)n/(double)fs;
  for (j=0; j<Nmaterial; j++) {
    ybuf[0] = e[0];
    y[j][0] = -ydb[j][0]*ybuf[0];
    for (n=1; n<Np; n++) {
      ybuf[n] = e[n]-yda[j][n]*ybuf[n-1];
      y[j][n] = ydb[j][n]*(ybuf[n]+ybuf[n-1]);
    }
  }

// ---------------------
// Resonant Filter
// ---------------------
  Nres = Timbre_Paraset(f0,Q,ds,&b2,id);
  printf("No_resonance=%d b2=%lf\n",Nres,b2);

  for (j=0; j<Nmaterial; j++) {
    for (n=0; n<Np; n++) s[j][n] = 0.;
    for (k=0; k<Nres; k++) {
      resonant_IIR2nd_design(f0[k],Q[k],(double)fs,&a1[j],
                             &a2[j]);
      resonant_IIR_delay(y[j],a1[j],a2[j],b2,ds[k],Np,ybuf,
                         yres);
      for (n=0; n<Np; n++) {
        s[j][n] += yres[n];
      }
    }
    Ampmax = -32768.;
    Ampmin =  32767.;
    for (n=0; n<Np; n++) {
      if (s[j][n]>Ampmax) Ampmax = s[j][n];
      if (s[j][n]<Ampmin) Ampmin = s[j][n];
    }
    if (Ampmax< abs(Ampmin)) Ampmax = abs(Ampmin);
    for (n=0; n<Np; n++) ibuf[n] = 
      (short)(s[j][n]/Ampmax*maxAmp); // 波形の正規化
// for (n=0;n<Np;n++) ibuf[n] = (short)(y[j][n]); // Materialの
                                                  // 出力
    strcat(strcpy(fn,fncommon),material[j]);
    printf("fn=%s\n",fn);
    wavwrite(&bufhead,1,fs,bits,Np,fn,fmt_id);
  }
  /*----------- メモリの解放 --------- */
  free(s);
  free(ybuf);
  free(y);
  free(bufhead.data);
  free(e);
  free(ibuf);
  free(xd);
  free(yda);
  free(ydb);
  free(yres);
  free(fn);
  free(fncommon);
  return(0);
}
// -------------------------------------------------------
int resonant_IIR2nd_design(double f0,double Q,double Fs,
                           double *a1,double *a2)
{
  double pi,alpha;
  pi = M_PI;
  alpha = 2.*pi*f0/Q;
  *a1 = (-2.)*exp(-alpha*(1./Fs))*cos(2.*pi*f0/Fs);
  *a2 = exp(-alpha*2./Fs);
  return(0);
}
// ----------------------------------------
int resonant_IIR_delay(double e[], double a1, double a2,
                       double b2, int ds, int Np, 
                       double ybuf[], double y[])
{
  int n;
  for (n=0; n<Np; n++) ybuf[n] = 0.0;
  ybuf[0] = e[0];
  y[0] = 0.;
  ybuf[1] = e[1]-a1*ybuf[0];
  y[1] = 0.; // ここの正確な計算は省略
  for (n=2; n<Np; n++) {
    ybuf[n]=e[n]-a1*ybuf[n-1]-a2*ybuf[n-2];
    if (n>=(2+ds+1))
      y[n] = (ybuf[n-ds] + b2*ybuf[n-2-ds]);
    else if (n== (2+ds) || n== (1+ds))
      y[n] = ybuf[n-ds];
    else
      y[n] = 0;
  }
  return (0);
}
