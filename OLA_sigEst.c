#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <wavlib.h>
#include <time.h>
#include "mhamwdw1.c"
int main(int argc, char *argv[])
{
  int nwdw,fs,nfft;
  int nshiftA,nshiftS,ntS;
  int m,j,i,I,ie,Ndata,n_dataA,n_dataS,nposA,nposS;
  int ndataSmax,nsecmax,nframemax;
  int id_ph;
  double *mham,*wbuf,*xbuf,*xbuf0,*xbufIm,ratio,pi;
  double *Xmabs,**Ymabs,*theta,*S;
  short *ixbuf,*iwbuf,izbuf;
  wav_head bufhead;
  char *fnI,*fnO;
  time_t timer; // time_t: long
  FILE *fdi,*fdo;

  extern void mhamwdw1(double *wdw,int nwdw,int nshiftS);
  extern int fft(int n,double x[],double y[]);
  if(argc != 3) {
    printf("\nusage: OLA_sigEst I_fn  O_fn\n");
    printf("  I_fn   : Input sound file name\n");
    printf("  O_fn   : Output sound file name\n");
    printf("ex: OLA_sigEst Wave001.wav data1.wav\n");
    printf("    gives two times as long as original sound.\n");
    return 0;
  }
  fnI = calloc(100,sizeof(char));
  fnO = calloc(100,sizeof(char));
  sscanf(argv[1],"%s",fnI);
  sscanf(argv[2],"%s",fnO);
  nsecmax = 20;  // 最大音声長〔s〕
  nfft = 2048;   // FFT点数
  fs   = 44100;  // サンプリング周波数〔Hz〕
  nwdw = 1440;   // 窓長
  nshiftA = 360; // 分析用フレームシフト．通常nwdw/4
  I = 300;       // maximum number of iteration
  ratio = 2.0;   // 合成用フレームシフトと分析用フレームシフトの比
  id_ph = 0;     // 初期位相の指標
                 // 0:原音の分析結果より　1:ランダム　2:ゼロ

  pi = M_PI; // π
  ndataSmax = fs*nsecmax;                  // 最大信号データ数
  nshiftS = (int)((double)nshiftA*ratio);  // 合成用フレームシフト
  ntS = nwdw-nshiftS;

  iwbuf = calloc(nwdw,sizeof(short));      // 各種バッファ割り当て
  ixbuf = calloc(ndataSmax,sizeof(short));
  if(nshiftS<=nwdw) 
       xbuf = calloc(nwdw,sizeof(double)); // 実数演算用バッファ
  else xbuf = calloc(nshiftS,sizeof(double));
  mham   = calloc(nwdw,sizeof(double));    // 修正ハミング窓データ
  wbuf   = calloc(nfft,sizeof(double));
  xbuf0  = calloc(nfft,sizeof(double));
  xbufIm = calloc(nfft,sizeof(double));
  theta  = calloc(nfft,sizeof(double));
  Xmabs  = calloc(nfft,sizeof(double));
  S      = calloc(I,   sizeof(double));
  time(&timer);

  wavread(&bufhead,fnI);  // 信号データアドレスはbufhead.data
  n_dataA = bufhead.Nbyte/bufhead.bl_size;
  nframemax = (int)((double)n_dataA/(double)nshiftA+1.0);

  Ymabs = calloc(nframemax, sizeof(double*)); // Ymabs[m][j]
  for(j=0; j<nframemax; j++) 
    Ymabs[j] = calloc(nfft,sizeof(double));
  mhamwdw1(mham,nwdw,nshiftS); // modified hamming window

  /* ======================================================= */
  // initialize
  srand(timer);
  for(m=nposA=nposS=0; ; m++) { // loop for frame
    if((n_dataA-nposA)<nwdw) {
      for(j=0; j<n_dataA-nposA; j++) 
        xbuf0[j] = (double)bufhead.data[nposA+j];
      for(j=n_dataA-nposA; j<nwdw; j++) xbuf0[j] = 0.;
    } else for(j=0; j<nwdw; j++) 
      xbuf0[j] = (double)bufhead.data[nposA+j];
    if(nfft>=nwdw)for(j=nwdw;j<nfft;j++) wbuf[j]=0.;// ゼロ詰め
    for(j=0; j<nfft; j++) xbufIm[j] = 0.0;

// 処理：
    for(j=0; j<nwdw; j++) 
      wbuf[j] = mham[j]*xbuf0[j]; // 窓と信号との積和演算
    fft(nfft,wbuf,xbufIm);
    for(j=0; j<=nfft/2; j++) {
      Ymabs[m][j] = sqrt(wbuf[j]*wbuf[j] + xbufIm[j]
                         *xbufIm[j]);   // |Ym(mS,ω)|の計算
      // giving random phase process is necessary
      // Ym = Ymabs[m][j]*cos(theta[m][j]), 
      //      j*Ymabs[m][j]*sin(theta[m][j])
      if(id_ph==1) theta[j] = 
        (2.*(double)rand()/32768. - 1.)*pi; // 位相 ランダム
      if(id_ph==2) theta[j] = 0.0;          // 位相 0
      if(id_ph!=0) {
        wbuf[j]  = Ymabs[m][j]*cos(theta[j]);
        xbufIm[j] = Ymabs[m][j]*sin(theta[j]);
      }
      if(j>=1 && j<nfft/2){ // nfft/2+1番目からnfft-1番目までの
                            // 値は複素共役として前半からコピー
        Ymabs[m][nfft-j] = Ymabs[m][j];
        wbuf[nfft-j]     = wbuf[j];
        xbufIm[nfft-j]   = -xbufIm[j];
      }
    }
    fft(-nfft,wbuf,xbufIm); // ifft  -> ym
    // overlapadd
    for(j=0; j<nwdw; j++) {
      if(j<ntS) {
        xbuf[j] = wbuf[j]*mham[j]+xbuf[j+nshiftS];
      } else {
        xbuf[j] = wbuf[j]*mham[j];
      }
    }
    if(nshiftS>nwdw) {
      for(j=nwdw; j<nshiftS; j++) {
        xbuf[j] = 0.0;
      }
    }
    for(j=0; j<nshiftS; j++) ixbuf[j+nposS] = (short)xbuf[j];
    nposA += nshiftA;
    nposS += nshiftS;
    if(nposA>=n_dataA) break;
  }
  /* ======================================================= */

  n_dataS = n_dataA = nposS;
  bufhead.data = ixbuf;
  nshiftA = nshiftS;
// 一般反復過程
  for(i=1; i<I; i++) {
    S[i-1] = 0.0;
    for(m=nposA=nposS=0; ; m++) {
      if((n_dataA-nposA)<nwdw) {
        for(j=0; j<n_dataA-nposA; j++) 
          xbuf0[j] = (double)bufhead.data[nposA+j];
        for(j=n_dataA-nposA; j<nwdw; j++) xbuf0[j] = 0.;
      } else for(j=0; j<nwdw; j++) xbuf0[j] = 
        (double)bufhead.data[nposA+j];

// 処理：
      // Xm導出と前段(i-1)反復時の評価
      for(j=0; j<nwdw; j++) 
        wbuf[j] = mham[j]*xbuf0[j];  // 窓と信号との積和演算
      if(nfft>=nwdw) for (j=nwdw; j<nfft; j++) 
        wbuf[j] = 0.;                      // 実部残りのゼロ詰め
      for(j=0; j<nfft; j++) xbufIm[j]=0.0; // 虚部　ゼロ詰め
      fft(nfft,wbuf,xbufIm);
      for(j=0; j<nfft; j++) {
        Xmabs[j] = sqrt(wbuf[j]*wbuf[j]
                   +xbufIm[j]*xbufIm[j]);  // Xm絶対値
        S[i-1] += (Ymabs[m][j]-Xmabs[j])
                  *(Ymabs[m][j]-Xmabs[j]); // 評価
        if(wbuf[j]!=0.0) 
          theta[j] = atan2(xbufIm[j],wbuf[j]); // phase deriva
                                               // -tion(θ)
        else theta[j] = pi/2.;
        wbuf[j]   = Ymabs[m][j]*cos(theta[j]);
        xbufIm[j] = Ymabs[m][j]*sin(theta[j]);
      }
      // X^i_ω hat derivation
      fft(-nfft,wbuf,xbufIm); // ifft of X^i_ω hat
      // overlapadd
      for(j=0; j<nwdw; j++) {
        if(j<ntS) {
          xbuf[j] = wbuf[j]+xbuf[j+nshiftS];
        } else {
          xbuf[j] = wbuf[j];
        }
      }
      if(nshiftS>nwdw) {
        for(j=nwdw; j<nshiftS; j++) {
          xbuf[j] = 0.0;
        }
      }
      for(j=0; j<nshiftS; j++) ixbuf[j+nposS] = (short)xbuf[j];
      nposA += nshiftA;
      nposS += nshiftS;
      if(nposA>=n_dataA) break;
    }  // m のループ
    S[i-1] /= (double)nfft;
  } // iのループ
  /* ======================================================= */
  printf("Iteration: %d\n",I);
  for(i=0; i<I; i++) {
    printf("%lf, ",S[i]/10000000000.0);
    if((i+1)%10==0)printf(" ...\n"); // Matlab仕様
  }
  wavwrite(&bufhead, bufhead.Nch, bufhead.fs,bufhead.bit,
           n_dataS, fnO, 1);
  free(iwbuf);
  free(ixbuf);
  free(mham);
  free(wbuf);
  free(xbuf);
  free(theta);
  free(Xmabs);
  free(Ymabs);
  free(S);
  return(0);
}
