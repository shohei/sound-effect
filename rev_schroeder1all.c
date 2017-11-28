#include <stdio.h>
#include <stdlib.h>
#include <wavlib.h>
#include <math.h>
#include <time.h>
#define NTAP 10

int main(int argc, char *argv[])
{
  char *fnI,*fnO;
  wav_head bufhead;
  short *ix,*ix1;
  double *x,*y,*y0,*y1,*y2,*y3,*ysum,*yout,*yout0;
  double g[6],delaysec[6],a[NTAP],delaysec0[NTAP];
  double sum,rev,dlyT,temp,amp[3];
  double fluctdelay0,fluctdelay,fluctamp0,fluctamp;
  int irandmax;
  int i,j,jj,n,fr,read,reminder,id,ie;
  int n_data,n_ext,ls,ntap;
  int fs,d[6],d0[NTAP],iratio,dmax,dmax0; // 遅延サンプル数

  void FIRcomb(short ix[], double x[], int d, int ls, 
               int n_data, int id);
  void IIRcomb_revOnly(short ix[], double x[], double y[],
                       double yout[], double g, int d,
                       int ls, int n_data, int n_ext, int id);
  void Allpass_rev(double xmid[], double x[], double y[],
                   short ix[], double xout[], double a,
                   int d, int ls, int n_data, int id);
  int MTdelay0(short ix[], double x[], double yout[], 
               double a[], int d[],int n_dbuf,int ntap,
               int n_data);

  if(argc != 3) {
    printf("\nusage: rev_schroeder1all I_filename ");
    printf("O_filename\n");
    printf("  I_filename   : Input  sound file name\n");
    printf("  O_filename   : Output sound file name\n");
    return 0;
  }
  fnI = calloc(100,sizeof(char));
  fnO = calloc(100,sizeof(char));
  sscanf(argv[1],"%s",fnI);
  sscanf(argv[2],"%s",fnO);

  wavread(&bufhead,fnI);
  n_data = bufhead.Nbyte/bufhead.bl_size;

  fs = bufhead.fs;
  ls = 8192; // 1回に処理するバッファ長
  ntap = NTAP;
  // Schroeder's reverberation simulator
// 初期反射のパラメータ設定
  srand((unsigned)time(NULL));
  irandmax = RAND_MAX;
  fluctdelay0 = 0.002;
  fluctamp0 = 0.1;
  for(j=0; j<ntap; j++) {
    fluctdelay = (-fluctdelay0)+(temp=rand())
                 *(2.*fluctdelay0)/(irandmax+1.);
    fluctamp = (-fluctamp0)+(temp=rand())
               *(2.*fluctamp0)/(irandmax+1.);
    a[j] = 0.6+fluctamp+(0.3-0.6)*(double)j/(double)ntap;
    delaysec0[j] = 0.020+(0.008+fluctdelay)*(double)j;
    d0[j] = (int)((double)fs*delaysec0[j]+0.5);
  }

  for(j=dmax0=0; j<ntap; j++) {
    dmax0 = (dmax0>d0[j]? dmax0 : d0[j]);
  }
// 残響のパラメータ設定
  g[0] = 0.871402;
  g[1] = 0.882762;
  g[2] = 0.891443;
  g[3] = 0.901117;

  delaysec[0] = 0.03985;  //〔s〕
  delaysec[1] = 0.03610;  //〔s〕
  delaysec[2] = 0.03327;  //〔s〕
  delaysec[3] = 0.03015;  //〔s〕
  delaysec[4] = 0.005;    //〔s〕
  delaysec[5] = 0.0017;   //〔s〕

  for(j=0; j<6; j++) {
    d[j] = (int)((double)fs*delaysec[j]+0.5);
  }
  g[4] = 0.7;
  g[5] = 0.7;
  dlyT = 0.0;
  for(j=dmax=0; j<6; j++) {
    dmax = (dmax>d[j]? dmax : d[j]);
    dlyT = dlyT>(temp= delaysec[j]*(-3.)/log10(g[j])) ?  
           dlyT : temp;
  }
  n_ext = (int)(dlyT*(double)fs);

// 総合系の振幅　直接音,初期反射,残響
  amp[0] = 0.3;
  amp[1] = 0.3;
  amp[2] = 0.3;
  x     = calloc(ls+dmax,sizeof(double)); // dmaxサンプル分
                                          // 遅延が可能
  y     = calloc(ls+dmax,sizeof(double)); // dmaxサンプル分
                                          // 遅延が可能
  y0    = calloc(n_data+n_ext,sizeof(double));
  y1    = calloc(n_data+n_ext,sizeof(double));
  y2    = calloc(n_data+n_ext,sizeof(double));
  y3    = calloc(n_data+n_ext,sizeof(double));
  ysum  = calloc(n_data+n_ext,sizeof(double));
  yout0 = calloc(n_data+dmax0,sizeof(double));
  yout  = calloc(n_data+n_ext,sizeof(double));
  ix = bufhead.data; // ファイルで読み込んだアドレスをixと
                     // 読み変える．名称の便宜上
  ix1 = calloc(n_data+n_ext,sizeof(short));

  if((ie=MTdelay0(ix,x,yout0,a,d0,dmax0,ntap,n_data))>0) {
    printf("The number of clipped samples: %d\n",ie);
  }

  IIRcomb_revOnly(ix,x,y,y0,g[0],d[0],ls,n_data,n_ext,1);
  IIRcomb_revOnly(ix,x,y,y1,g[1],d[1],ls,n_data,n_ext,1);
  IIRcomb_revOnly(ix,x,y,y2,g[2],d[2],ls,n_data,n_ext,1);
  IIRcomb_revOnly(ix,x,y,y3,g[3],d[3],ls,n_data,n_ext,1);
  for(j=0; j<n_data+n_ext; j++) 
    ysum[j] = y0[j]+y1[j]+y2[j]+y3[j];

  iratio = (int)(0.1/delaysec[4]);
  if ((rev=pow(g[4],(double)(iratio-1))*(1.0-g[4]*g[4]))>0.001)

  {
    printf("Error. Relative level = %lf at 100ms pt. ",rev);
    printf("should be smaller than 0.001.\n");
    printf("       Change 'g' to smaller value. ");
    printf("(Rev. time should be within 100ms.)\n");
    return -1;
  }
  Allpass_rev(ysum,x,y,ix1,yout,g[4],d[4],ls,n_data+n_ext,0);
  Allpass_rev(yout,x,y,ix1,ysum,g[5],d[5],ls,n_data+n_ext,0);

  for(j=0; j<n_data; j++) ix1[j] = (short)(amp[0]*(double)ix[j]) 
    + (short)(amp[1]*yout0[j] + amp[2]*ysum[j]);
  for(j=n_data; j<n_data+dmax0; j++) 
    ix1[j] = (short)(amp[1]*yout0[j] + amp[2]*ysum[j]) ;
  for(j=n_data+dmax0; j<n_data+n_ext; j++) 
    ix1[j] = (short)(amp[2]*ysum[j]) ;
  bufhead.data = ix1;
  wavwrite(&bufhead, bufhead.Nch, bufhead.fs,
           bufhead.bit, n_data+n_ext, fnO, 1);
  free(x);
  free(y);
  free(y0);
  free(y1);
  free(y2);
  free(y3);
  free(ysum);
  free(yout);
  free(bufhead.data);
  return 0;
}
void IIRcomb_revOnly(short ix[], double x[], double y[],
                     double yout[],double g, int d, 
                     int ls,int n_data,int n_ext,int id)
{
  int i,fr,j,jj,read,reminder,n,mode;
  double sum;
// d: delay in number of samples
  // id:  1: additive comb filtering  
  //     -1: subtractive comb filtering
  //      0: no action
  mode = 0;
  for(i=0; i<d; i++) {
    x[i] = y[i] = 0.0;
  }
  for(fr=jj=0 ; ; fr++ ) {
    read = fr*ls;
    n = ls;
    reminder = (mode==0? n_data-read : n_data+n_ext-read);
    if(reminder<ls) {
      n = reminder;
      mode = mode==0? 0 : 2; // n=0も含む
    }
    // ファイルからの読み込みの場合はここに挿入
    // if((n=fread( (char *)ix,2,ls,input))<ls)最終処理
    if(mode==0) {
      for(i=0; i<n; i++) {
        x[i+d] = (double)ix[read+i];
      }
      for(i=n; i<ls; i++) {
        x[i+d] = 0.;
      }
      if(n<ls) mode = 1;
      n = ls;
    } else {
      for(i=0; i<n; i++) {
        x[i+d] = 0.;
      }
    }
    for(i=0; i<n; i++) { // フィルタリング処理
      yout[jj++] = (y[(d+i)%ls]= x[i]+ (double)id*g*y[i]);
    }
    // ファイルに出力する場合はここに挿入
    // fwrite((char *)ix,2,n,output);　など
    // 最後のd個を次の計算にも用いるため，フィルタリング処理後コピー
    for(i=0; i<d; i++) x[i] = x[ls+i];
    if(mode==2)break;
  }
  return;
}
void Allpass_rev(double xmid[], double x[], double y[],
                 short ix[], double xout[], double g,
                 int d, int ls, int n_data, int id)
{
  int i,fr,j,jj,read,reminder,n;
  double sum;
// d: delay in number of samples
// id: output: 0/1=double/short (input is always double)
  for(i=0; i<d; i++) {
    x[i] = y[i] = 0.0;
  }
  for(fr=jj=0 ; ; fr++ ) {
    read = fr*ls;
    reminder = n_data-read;
    n = ls;
    if(reminder<ls) {
      n = reminder;
    }
    if(n==0)break;
    // ファイルからの読み込みの場合はここに挿入
    // if((n=fread( (char *)ix,2,ls,input))<ls)最終処理

    for(i=0; i<n; i++) {
      x[i+d] = xmid[read+i];
    }
    for(i=0; i<n; i++) { // フィルタリング処理 
                         // x[i+d]が現在のx(数式のx(n))
      if(id==1) {
        ix[jj++] = (short)(y[(d+i)%ls]=x[i]-g*x[i+d]+g*y[i]);
      } else {
        xout[jj++] = (y[(d+i)%ls]=x[i]-g*x[i+d]+g*y[i]);
      }
    }
    // ファイルに出力する場合はここに挿入
    // fwrite((char *)ix,2,n,output);　など
    // 最後のd個を次の計算にも用いるため，コピー
    for(i=0; i<d; i++) x[i] = x[ls+i];
    if (reminder<ls) break;
  }
  return;
}
int MTdelay0(short ix[], double x[], double yout[], double a[],
             int d[], int n_dbuf, int ntap, int n_data)
{
  int i,fr,j,jj,read,reminder,n,clip,dmax;
  double sum;
// d: delay in number of samples
// return value: the number of clipped samples

  clip = 0;
  for(j=dmax=0; j<ntap; j++) dmax = (dmax>d[j]? dmax : d[j]);
  for(i=0; i<dmax; i++) {
    x[i] = 0.0;
  }
  for(fr=jj=0 ; ; fr++ ) {
    read = fr*n_dbuf;
    n = n_dbuf;
    reminder = n_data-read ;
    if(reminder<n_dbuf) n = reminder;
    // ファイルからの読み込みの場合はここに挿入
    // if((n=fread( (char *)ix,2,dmax,input))<dmax)最終処理

    for(i=0; i<n; i++) { // ディレイライン処理
      yout[jj] = 0.0;    // 直接音は加算せず
      for (j=0; j<ntap; j++) {
        yout[jj] += a[j]*x[(i+n_dbuf-d[j])%n_dbuf];
      }
      x[(i+dmax)%n_dbuf] = (double)ix[read+i];
      if(abs((int)yout[jj])> 32767) clip += 1;
      jj++;
    }

    if(n<n_dbuf) {
      for(i=n; i<n+dmax; i++) {
        yout[jj] = 0.;
        for (j=0; j<ntap; j++) {
          if(i>=dmax) x[(i+dmax)%n_dbuf] = 0.0;
          yout[jj] += a[j]*x[(i+n_dbuf-d[j])%n_dbuf];
        }
        if(abs((int)yout[jj])> 32767) clip += 1;
        jj++;
      }
    }
    // ファイルに出力する場合はここに挿入
    // fwrite((char *)ix,2,n,output);　など
    if(n<n_dbuf)break;
  }
  return clip;
}
