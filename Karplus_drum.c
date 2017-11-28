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
    double b;

    if(argc != 7) {
        printf("\nusage: Karplus_drum n_data amp f fs filename\n");
        printf("   n_data     : the number of data\n");
        printf("   amp        : amplitude\n");
        printf("   b          : probability (real) ex. 0.8: metalic, 0.5: snare, 0.2:other\n");
        printf("   f          : update cycle (originally pitch frequency) in Hz (real) \n");
        printf("   fs         : sampling frequency in Hz (real)\n");
        printf("   filename   : file name of sound\n");
        printf(" note: if no._of_data is given in negative,\n");
        printf("        output file is 'raw' format. Otherwize 'wav' format\n");
        return 0;
    }

    n_data_id = atoi(argv[1]);
    n_data=abs(n_data_id);
    fmt_id= n_data/n_data_id;
    irandmax=RAND_MAX;


    buffer=calloc(n_data,sizeof(double));
    ibuf=bufhead.data=calloc(n_data,sizeof(short));
    fn=calloc(100,sizeof(char));

    amp  = atof(argv[2]);
    b    = atof(argv[3]);
    f    = atof(argv[4]);
    fs = atof(argv[5]);
    sscanf(argv[6],"%s",fn);
    wavt_len=(int)(fs/f);
    srand((unsigned)time(NULL));
    printf("irandmax=%d amp=%lf wavt_len=%d\n",irandmax,amp,wavt_len);

    for(i=0; i<=wavt_len; i++)  /* generation of white noise */
        ibuf[i]=(short)(buffer[i] =(-amp)+ rand()*(2.*amp)/(irandmax+1.));
    for(i=wavt_len+1; i<n_data; i++)
        ibuf[i]=(short)(buffer[i]= ((double)rand()/(double)irandmax>b?
          0.5 : (-0.5)) *(buffer[i-wavt_len]+buffer[i-wavt_len-1]));

    wavwrite(&bufhead,1,fs,bits,n_data,fn,fmt_id);

    free(buffer);
    free(bufhead.data);
    free(fn);
    return 0;
}
