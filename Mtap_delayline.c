#include <stdio.h>
#include <stdlib.h>
#include <wavlib.h>
#include <math.h>
#define NTAP 6

int main(int argc, char *argv[])
{
  char *fnI,*fnO;
  wav_head bufhead;
  short *ix,*ix1;
  double *x,*yout;
  double a[NTAP], delaysec[NTAP];
  double sum,rev;
  int i,j,jj,n,fr,read,reminder,id,ie;
  int ntap,n_data;
  int fs,d[NTAP],iratio,dmax; // ’x‰„ƒTƒ“ƒvƒ‹”

  int MTdelay(short ix[],double x[],double yout[],double a[],
              int d[],int n_dbuf,int ntap,int n_data);

  if(argc != 3) {
    printf("\nusage: Mtap_delayline I_filename O_filename\n");
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
  ntap = NTAP;
  fs = bufhead.fs;

  a[0] = 0.3;
  a[1] = 0.4;
  a[2] = 0.5;
  a[3] = 0.6;
  a[4] = 0.7;
  a[5] = 0.8;

  delaysec[0] = 1.8;  //ksl
  delaysec[1] = 1.5;  //ksl
  delaysec[2] = 1.3;  //ksl
  delaysec[3] = 0.7;  //ksl
  delaysec[4] = 0.5;  //ksl
  delaysec[5] = 0.2;  //ksl
  for(j=0; j<ntap; j++) {
    d[j] = (int)((double)fs*delaysec[j]+0.5);
  }

  for(j=dmax=0; j<ntap; j++) {
    dmax = (dmax>d[j]? dmax : d[j]);
  }

  x    = calloc(dmax,sizeof(double)); // dmaxƒTƒ“ƒvƒ‹•ª’x‰„‚ª‰Â”\
  yout = calloc(n_data+dmax,sizeof(double));
  ix = bufhead.data; // ƒtƒ@ƒCƒ‹‚Å“Ç‚İ‚ñ‚¾ƒAƒhƒŒƒX‚ğix‚Æ
                     // “Ç‚İ•Ï‚¦‚éD–¼Ì‚Ì•Ö‹Xã
  ix1 = calloc(n_data+dmax,sizeof(short));

  if((ie=MTdelay(ix,x,yout,a,d,dmax,ntap,n_data))>0) {
    printf("The number of clipped samples: %d\n",ie);
  }
  for(j=0; j<n_data+dmax; j++) ix1[j] = (short)yout[j] ;
  bufhead.data = ix1;
  wavwrite(&bufhead, bufhead.Nch, bufhead.fs,
           bufhead.bit, n_data+dmax, fnO, 1);
  free(x);
  free(yout);
  free(bufhead.data);
  return 0;
}
/* -------------------------------------------------------- */
int MTdelay(short ix[],double x[],double yout[],double a[],
            int d[],int n_dbuf,int ntap,int n_data)
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
    // ƒtƒ@ƒCƒ‹‚©‚ç‚Ì“Ç‚İ‚İ‚Ìê‡‚Í‚±‚±‚É‘}“ü

    for(i=0; i<n; i++) { // ƒfƒBƒŒƒCƒ‰ƒCƒ“ˆ—
      yout[jj] = (double)ix[read+i]; // Œ´‰¹(’x‰„‚È‚µ)‚ğ‰ÁZ
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
    // ƒtƒ@ƒCƒ‹‚Éo—Í‚·‚éê‡‚Í‚±‚±‚É‘}“ü
    if(n<n_dbuf)break;
  }
  return clip;
}
