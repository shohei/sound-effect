#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "wavlib.h"

int main(int argc, char *argv[])
{
  int n_data,n_data_id,fmt_id,i;
  double f,fs,amp, *buffer;
  short *ibuf;
  char *fn;
  int irandmax;
  wav_head bufhead;
  int wavt_len,taper_len;
  double taper_ms,temp;

  if(argc != 6) {
    printf("\nusage: Karplus n_data amp f fs filename\n");
    printf("   n_data     : the number of data\n");
    printf("   amp        : amplitude\n");
    printf("   f          : pitch frequency in Hz (real)\n");
    printf("   fs         : sampling frequency in Hz (real)\n");
    printf("   filename   : file name of sound\n");
    printf(" note: if no._of_data is given in negative,\n");
    printf("        output file is 'raw' format. ");
    printf("Otherwize 'wav' format\n");
    return 0;
  }
  taper_ms = 0.15;
  n_data_id = atoi(argv[1]);
  n_data = abs(n_data_id);
  fmt_id = n_data/n_data_id;
  irandmax = RAND_MAX;

  buffer = calloc(n_data,sizeof(double));
  ibuf = bufhead.data = calloc(n_data,sizeof(short));
  fn = calloc(100,sizeof(char));

  amp  = atof(argv[2]);
  f    = atof(argv[3]);
  fs   = atof(argv[4]);
  sscanf(argv[5],"%s",fn);
  wavt_len = (int)(fs/f);
  taper_len = taper_ms*fs;
  srand((unsigned)time(NULL));
  printf("irandmax=%d amp=%lf wavt_len=%d taper_len=%d\n",
         irandmax,amp,wavt_len,taper_len);

  for(i=0; i<=wavt_len; i++)  /* generation of white noise */
    ibuf[i] = (short)(buffer[i] =
      (-amp)+ rand()*(2.*amp)/(irandmax+1.));
  for(i=wavt_len+1; i<n_data; i++)
    ibuf[i] = (short)(buffer[i]=0.5*(buffer[i-wavt_len]
                                     +buffer[i-wavt_len-1]));
  for(i=n_data - taper_len; i<n_data; i++)
    ibuf[i] = (short)(buffer[i]*((double)(i-n_data+taper_len)*
                                 (-1./(double)taper_len) + 1.0));

  wavwrite(&bufhead,1,fs,bits,n_data,fn,fmt_id);

  free(buffer);
  free(bufhead.data);
  free(fn);
  return 0;
}
