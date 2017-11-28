#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "wavlib.h"

int main(int argc, char *argv[])
{
    int n_data,n_data_id,fmt_id,i,irandmax,funcN;
    double f,fs,amp,I,cons,cons1,cons2,xdash, *buffer;
    double fc,fm;
    short *ibuf;
    char *fn;
    double pi; /* =3.14159265358979323846 */
    wav_head bufhead;

    if(argc != 9) {
        printf("\nusage: func_gen1_v1 func#  n_data  amp  f(fc)  fm  I  fs  filename\n");
        printf("  func#:  0: sine, 1: w_noise, 2: RM, 3: AM, 4: FM, 5: pulse\n");
        printf("  n_data     : the number of data\n");
        printf("  amp        : amplitude\n");
        printf("  f/fc       : signal frequency/carrier frequency in Hz \n");
        printf("  fm         : modulation frequency in Hz \n");
        printf("  I          : Index in FM synthesis \n");
        printf("  fs         : sampling frequency in Hz \n");
        printf("  filename   : file name of sound\n\n");
        printf(" Notes:  Supply all the arguments with dummy even if not necessary.\n");
        printf("        if no._of_data is given in negative,\n");
        printf("        output file is 'raw' format. Otherwize 'wav' format\n");
        return 0;
    }
    pi=M_PI;
    n_data_id = atoi(argv[2]);
    n_data=abs(n_data_id);
    fmt_id= n_data/n_data_id;

    buffer=calloc(n_data,sizeof(double));
    ibuf=bufhead.data=calloc(n_data,sizeof(short));
    fn=calloc(100,sizeof(char));

    funcN= atoi(argv[1]);
    amp  = atof(argv[3]);
    f    = atof(argv[4]);
    fm   = atof(argv[5]);
    I    = atof(argv[6]);
    fs   = atof(argv[7]);
    fc   =f;
    sscanf(argv[8],"%s",fn);
    cons=fs/f;
    switch(funcN) {
    case 0: //  ê≥å∑îg
        for(i=0; i<n_data; i++)
            ibuf[i]=(short)(buffer[i] = amp*sin(2.*pi*f*(double)i/fs));
        break;
    case 1: //  îíéGâπ
        srand((unsigned)time(NULL));
        printf("irandmax=%d amp=%lf\n",irandmax,amp);
        for(i=0; i<n_data; i++)
            ibuf[i]=(short)(buffer[i] =(-amp)+ rand()*(2.*amp)/(irandmax+1.));
        break;
    case 2: // RM
        cons1=cons/2.;
        cons2=4.*amp*f/fs;
        for(i=0; i<n_data; i++)
            ibuf[i]=(short)(buffer[i] = amp*sin(2.*pi*fm*(double)i/fs)
                                        *sin(2.*pi*fc*(double)i/fs) );
        break;
    case 3: // AM
        cons2=2.*amp*f/fs;
        for(i=0; i<n_data; i++)
            ibuf[i]=(short)(buffer[i] = amp*(0.5*0.5*sin(2.*pi*fm*(double)i/fs))
                                        *sin(2.*pi*fc*(double)i/fs) );
        break;
    case 4: // FM
        cons1=cons/2.;
        for(i=0; i<n_data; i++)
            ibuf[i]=(short)(buffer[i] = amp*sin(2.*pi*fc*(double)i/fs
                                                +I*sin(2.*pi*fm*(1./fs)*(double)i)) ) ;
        break;

    }
    wavwrite(&bufhead,1,(long)fs,bits,n_data,fn,fmt_id);

    //  èIóπèàóù
    free(buffer);
    free(bufhead.data);
    free(fn);
    return 0;
}