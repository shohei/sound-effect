#include <stdio.h>
#include <stdlib.h>
#include <wavlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
  char *fnI,*fnO;
  wav_head bufhead;
  short *ix,*ix1;
  double *x,*yout;
  double g,delaysec;
  double sum,rev;
  int i,j,jj,n,fr,read,reminder,id,ie;
  int n_data;
  int fs,d,iratio,dmax; // 遅延サンプル数

  int delay(short ix[],double x[],double yout[],double a,
            int d,int n_dbuf,int n_data);

  if(argc != 3) {
    printf("\nusage: onetap_delay I_filename O_filename\n");
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
  g = 0.87;
  delaysec = 1.8;  //〔s〕
  d = (int)((double)fs*delaysec+0.5);
  dmax = d ;
  printf("dmax=%d\n",dmax);
  x    = calloc(dmax,sizeof(double)); // dmaxサンプル分遅延が可能
  yout = calloc(n_data+dmax,sizeof(double));
  ix = bufhead.data; // ファイルで読み込んだアドレスをixと
                     // 読み変える．名称の便宜上
  ix1 = calloc(n_data+dmax,sizeof(short));
  if((ie=delay(ix,x,yout,g,d,dmax,n_data))>0) {
    printf("The number of clipped samples: %d\n",ie);
  }
  for(j=0; j<n_data+d; j++) ix1[j] = (short)yout[j] ;
  bufhead.data = ix1;
  wavwrite(&bufhead, bufhead.Nch, bufhead.fs,
           bufhead.bit, n_data+d, fnO, 1);
  free(x);
  free(yout);
  free(bufhead.data);
  return 0;
}
/* ----------------------------------------------------- */
int delay(short ix[],double x[],double yout[],double a,
          int d,int n_dbuf,int n_data)
{
  int i,fr,j,jj,read,reminder,n,clip;
  double sum;
// d: delay in number of samples
// return value: the number of clipped samples

  clip = 0;
  for(i=0; i<d; i++) {
    x[i] = 0.0;
  }
  for(fr=jj=0 ; ; fr++ ) {
    read = fr*n_dbuf;
    n = n_dbuf;
    reminder = n_data-read ;
    if(reminder<n_dbuf) {
      n = reminder;
    }
    // ファイルからの読み込みの場合はここに挿入
    for(i=0; i<n; i++) { // ディレイライン処理
      yout[jj++] = (double)ix[read+i] + a*x[i];
      x[(i+d)%n_dbuf] = (double)ix[read+i];
      if(abs((int)yout[jj-1])> 32767) clip += 1;
    }
    if(n<n_dbuf) {
      for(i=n; i<n+d; i++) {
        yout[jj++] = a*x[i%n_dbuf];
        if(abs((int)yout[jj-1])> 32767) clip += 1;
      }
    }
    // ファイルに出力する場合はここに挿入
    if(n<n_dbuf)break;
  }
  return clip;
}
