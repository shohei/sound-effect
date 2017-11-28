#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "wavlib.h"

int main(int argc, char *argv[])
{
  int n_data,n_data_id,fmt_id,i,irandmax,funcN;
  double f,fs,amp,cons,cons1,cons2,xdash, *buffer;
  short *ibuf;
  char *fn;
  double pi; /* =3.14159265358979323846 */
  wav_head bufhead;

  if(argc != 7) {
    printf("\nusage: func_gen func#  n_data  amp  ");
    printf("f  fs  filename\n");
    printf("   func#:  0: sine, 1: w_noise, 2: trig, ");
    printf("3: sawtooth, 4: rectang, 5: pulse\n");
    printf("   n_data     : the number of data\n");
    printf("   amp        : amplitude\n");
    printf("   f          : signal frequency in Hz \n");
    printf("   fs         : sampling frequency in Hz \n");
    printf("   filename   : file name of sound\n");

    printf(" note: if no._of_data is given in negative,\n");
    printf("        output file is 'raw' format. ");
    printf("Otherwize 'wav' format\n");
    return 0;
  }
  pi = M_PI;
  n_data_id = atoi(argv[2]);
  n_data = abs(n_data_id);
  fmt_id = n_data/n_data_id;

  buffer = calloc(n_data,sizeof(double));
  ibuf = bufhead.data = calloc(n_data,sizeof(short));
  fn = calloc(100,sizeof(char));

  funcN = atoi(argv[1]);
  amp   = atof(argv[3]);
  f     = atof(argv[4]);
  fs    = atof(argv[5]);
  sscanf(argv[6],"%s",fn);
  cons = fs/f;
  switch(funcN) {
  case 0: //  ê≥å∑îg
    for(i=0; i<n_data; i++)
      ibuf[i] = (short)(buffer[i] = 
        amp*sin(2.*pi*f*(double)i/fs));
    break;
  case 1: //  îíéGâπ
    srand((unsigned)time(NULL));
    printf("irandmax=%d amp=%lf\n",irandmax,amp);
    for(i=0; i<n_data; i++)
      ibuf[i] = (short)(buffer[i] =
        (-amp)+ rand()*(2.*amp)/(irandmax+1.));
    break;
  case 2: //  éOäpîg
    cons1 = cons/2.;
    cons2 = 4.*amp*f/fs;
    for(i=0; i<n_data; i++)
      ibuf[i] = (short)(buffer[i] = 
        (xdash=fmod((double)i,cons)) <cons1?  cons2*xdash-amp : 
        (-cons2)*xdash+3.*amp);
    break;
  case 3: //  ãòîg
    cons2 = 2.*amp*f/fs;
    for(i=0; i<n_data; i++)
      ibuf[i] = (short)(buffer[i] =    
        cons2*(xdash=fmod((double)i,cons))-amp );
    break;
  case 4: //  ï˚å`îg
    cons1 = cons/2.;
    for(i=0; i<n_data; i++)
      ibuf[i] = (short)(buffer[i] =  
        (xdash=fmod((double)i,cons)) <cons1?  amp : -amp);
    break;
  case 5: //  ÉpÉãÉXîg
    cons1 = 1.;
    for(i=0; i<n_data; i++)
      ibuf[i] = (short)(buffer[i] =  
        (xdash=fmod((double)i,cons)) < cons1?  amp : 0.);
    break;
  }
  wavwrite(&bufhead,1,(long)fs,bits,n_data,fn,fmt_id);

  //  èIóπèàóù
  free(buffer);
  free(bufhead.data);
  free(fn);
  return 0;
}
