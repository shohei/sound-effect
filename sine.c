#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wavlib.h"

int main(int argc, char *argv[])
{
  int n_data,n_data_id,fmt_id,fs,i;
  double f,amp, *buffer;
  short *ibuf;
  char *fn;
  double pi; /* =3.14159265358979323846 */
  wav_head bufhead;

  if(argc != 6) {
    printf("\nusage: sine n_data, amp, f, fs, filename\n");
    printf("   n_data     : the number of data\n");
    printf("   amp        : amplitude\n");
    printf("   f          : signal frequency in Hz \n");
    printf("   fs         : sampling frequency in Hz\n");
    printf("   filename   : file name of sound\n");
    printf(" note: if no._of_data is given in negative,\n");
    printf("        output file is 'raw' format. ");
    printf("Otherwize 'wav' format\n");
    return 0;
  }
  pi        = M_PI;
  n_data_id = atoi(argv[1]);
  n_data    = abs(n_data_id);
  fmt_id    = n_data/n_data_id;

  buffer = calloc(n_data,sizeof(double));
  ibuf   = bufhead.data=calloc(n_data,sizeof(short));
  fn     = calloc(100,sizeof(char));

  amp  = atof(argv[2]);
  f    = atof(argv[3]);
  fs   = atoi(argv[4]);
  sscanf(argv[5],"%s",fn);

  for(i=0; i<n_data; i++)  /* generation of sine wave */
    ibuf[i] = (short)(buffer[i] = 
      amp*sin(2.*pi*f*(double)i/(double)fs));

  wavwrite(&bufhead,1,fs,bits,n_data,fn,fmt_id);

  /* ending message */
  free(buffer);
  free(bufhead.data);
  free(fn);
  return 0;
}
