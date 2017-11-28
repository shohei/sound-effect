#include <stdio.h>
#include <stdlib.h>
#include <wavlib.h>
#include <math.h>

int main(int argc, char *argv[])
{

    char *fnI,*fnO;
    wav_head bufhead;
    short *ix;
    double *ham1,*filtr,*x,*y;
    double delaysec;
    int i,j,jj,n,funcN,fr,ilast,read,reminder,id,ie;
    int n_data,ls;
    int d; //遅延サンプル数
    double sum;
    int nfilter,fc,fc0,fc1,fs;
    void FIRfilt(short ix[],double x[],double filtr[],
                 int nfilter,int ls,int n_data);
    void lpfdsn(double fc,double fs,int n,double wdw[],double f[]);
    void hpfdsn(double fc,double fs,int n,double wdw[],double f[]);
    void bpfdsn(double fleft,double fright,double fs,
                int n,double wdw[],double f[]);
    void befdsn(double fleft,double fright,double fs,
                int n,double wdw[],double f[]);
    void hamwdw1(double wdw[],int n);

    if(argc != 4) {
        printf("\nusage: filt_LPFapp func#  I_filename O_filename\n");
        printf(" func # | 0: FIR LPF, 1: FIR HPF, 2: FIR BPF, 3: FIR BEF\n");
        printf("        | 4: FIR Comb add, 5: FIR Comb subt,\n");
        printf("  I_filename   : Input  sound file name\n");
        printf("  O_filename   : Output sound file name\n");
        printf("  * Demo sound : Karplus.demo.wav,summer2006_4.wav, etc.\n");
        return 0;
    }
    funcN=atof(argv[1]);
    fnI=calloc(100,sizeof(char));
    fnO=calloc(100,sizeof(char));
    sscanf(argv[2],"%s",fnI);
    sscanf(argv[3],"%s",fnO);

    wavread(&bufhead,fnI);
    n_data=bufhead.Nbyte/bufhead.bl_size;
    printf("funcN=%d %s\n",funcN,fnI);

    ls= 8192; // 1回に処理するバッファ長
    nfilter=201;
    fs=44100;
    fc=3000;
    fc0=2000,fc1=4000;

    ham1=calloc(nfilter,sizeof(double));
    filtr=calloc(nfilter,sizeof(double));
    x=calloc(n_data,sizeof(double));
    y=calloc(n_data,sizeof(double));
    ix=bufhead.data; //ファイルで読み込んだアドレスをixと読み変える。名称の便宜上。

    hamwdw1(ham1,nfilter);
    switch(funcN) {
    case 0:  // FIR LPF
        lpfdsn((double)fc,(double)fs,nfilter,ham1,filtr);
        FIRfilt(ix,x,filtr,nfilter,ls,n_data); // FIRフィルタ処理
        break;
    case 1:  // FIR HPF
        hpfdsn((double)fc,(double)fs,nfilter,ham1,filtr);
        FIRfilt(ix,x,filtr,nfilter,ls,n_data);
        break;
    case 2:  // FIR BPF
        bpfdsn((double)fc0,(double)fc1,(double)fs,nfilter,ham1,filtr);
        FIRfilt(ix,x,filtr,nfilter,ls,n_data);
        break;
    case 3: // FIR BEF
        befdsn((double)fc0,(double)fc1,(double)fs,nfilter,ham1,filtr);
        FIRfilt(ix,x,filtr,nfilter,ls,n_data);
        break;
    }

    wavwrite(&bufhead, bufhead.Nch, bufhead.fs,
             bufhead.bit, bufhead.Nbyte/bufhead.bl_size, fnO, 1);
    free(ham1);
    free(filtr);
    free(x);
    free(y);
    free(bufhead.data);
    return 0;
}
/* --------------------------------------------------------------- */
void FIRfilt(short ix[],double x[],double filtr[],
             int nfilter,int ls,int n_data)
{   int i,fr,j,jj,read,reminder,n,ilast;
    double sum;

    for(i=0; i<nfilter; i++) {
        x[i]=0.0;
    }
    for(fr=jj=ilast=0 ; ; fr++ ) {
        read=fr*ls;
        reminder=n_data-read;
        n=ls;
        if(reminder<ls) {
            ilast=1;
            n=reminder;
        }
        if(n==0)break;
        // ファイルからの読み込みの場合はここに挿入
        // if((n=fread( (char *)ix,2,ls,input))<ls)ilast=1;

        for(i=0; i<n; i++) {
            x[i+nfilter]=(double)ix[read+i];
        }
        if(ilast==1) {
            for(i=0; i<ls-n; i++) {
                x[i+n+nfilter]=0.0;
            }
        }

        for(i=0; i<n; i++) { // フィルタリング処理
            sum=0.0;                     // 畳み込み
            for(j=0; j<nfilter; j++)sum += filtr[j]*x[i+(nfilter-1)-j];
            ix[jj++]=(short)sum;
        }
        // ファイルに出力する場合はここに挿入
        // fwrite((char *)ix,2,n,output);　など
        //最後のnfilter個を次の計算にも用いるため、コピー
        for(i=0; i<nfilter; i++) x[i]=x[ls+i];
        if (reminder<ls) break;
    }
    return;
}
/* --------------------------------------------------------------- */
void lpfdsn(double fc,double fs,int n,double wdw[],double f[])
{   int i;
    double d,x,pi=3.14159265358979323846264338327950288419716939;
    double alp,twopi,cons;
    /* --------------------------------------------- */
    twopi=2.0*pi;
    if(n>0)
    {   d=twopi*fc/fs;
        cons=2.0*fc/fs;
        alp=(double)(n-1)/2.0;

        /* -------------- first half of the calculation --------------- */
        for(i=0; i<(n-1)/2; i++)f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;

        /* -------------- last half  of the calculation --------------- */
        if((double)((n-1)/2) == alp )
        {   f[(n-1)/2]=wdw[(n-1)/2]*2.0*fc/fs;
            for(i=(n+1)/2; i<n; i++)f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
        }
        else
        {   for(i=(n-1)/2; i<n; i++)f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
        }
    }
    return ;
}
/* --------------------------------------------------------------- */
void hpfdsn(double fc,double fs,int n,double wdw[],double f[])
{   int i;
    double d,x,pi=3.14159265358979323846264338327950288419716939;
    double alp,twopi,cons;

    fc= fs/2.0 -fc;  /* fc conversion */
    twopi=2.0*pi;
    if(n>0)
    {   d=twopi*fc/fs;
        cons=2.0*fc/fs;
        alp=(double)(n-1)/2.0;

        /* -------------- first half of the calculation --------------- */
        for(i=0; i<(n-1)/2; i++)
        {   f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
            if(i/2*2 != i)  f[i] *= -1.0;
        }
        /* -------------- last half  of the calculation --------------- */
        if((double)((n-1)/2) == alp )
        {   f[(n-1)/2]=wdw[(n-1)/2]*2.0*fc/fs; //　nが奇数の場合
            for(i=(n+1)/2; i<n; i++)
            {   f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
                if(i/2*2 != i)  f[i] *= -1.0;
            }
        }
        else
        {   for(i=(n-1)/2; i<n; i++)
            {   f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
                if(i/2*2 != i)  f[i] *= -1.0;
            }
        }
    }
    return;
}
/* -------------------------------------------------------------- */
void bpfdsn(double fleft,double fright,double fs,int n,
            double wdw[],double f[])
{   int i;
    double d,x,pi=3.14159265358979323846264338327950288419716939;
    double fc,fcc,alp,twopi,cons;
    /* --------------------------------------------- */

    fcc=(fleft+fright)/2.0;
    fc=fright-fcc;
    twopi=2.0*pi;
    if(n>0)
    {   d=twopi*fc/fs;
        cons=2.0*fc/fs;
        alp=(double)(n-1)/2.0;

        /* -------------- first half of the calculation --------------- */
        for(i=0; i<(n-1)/2; i++)f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;

        /* -------------- last half  of the calculation --------------- */
        if((double)((n-1)/2) == alp )
        {   f[(n-1)/2]=wdw[(n-1)/2]*2.0*fc/fs;
            for(i=(n+1)/2; i<n; i++)f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
        }
        else
        {   for(i=(n-1)/2; i<n; i++)f[i]=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
        }
        // BPFへの変換
        cons=twopi*fcc/fs;
        for(i=0; i<n; i++)
        {   f[i] *= (alp=2.0*cos(cons*(double)i));
        }
    }
    return ;
}
/* -------------------------------------------------------------- */
void befdsn(double fleft,double fright,double fs,int n,double wdw[],double f[])
{   int i;
    double fc,fc1;
    double d,d1,x,pi=3.14159265358979323846264338327950288419716939;
    double alp,twopi,cons,cons1;
    /* --------------------------------------------- */
    fc=fleft;
    fc1= fs/2.0 -fright;  /* fc conversion */
    printf(" fleft=%lf fright=%lf fc=%lf fc1=%lf\n",fleft,fright,fc,fc1);
    twopi=2.0*pi;
    if(n>0)
    {   d=twopi*fc/fs;
        cons=2.0*fc/fs;
        alp=(double)(n-1)/2.0;
        d1=twopi*fc1/fs;
        cons1=2.0*fc1/fs;

        /* -------------- first half of the calculation --------------- */
        for(i=0; i<(n-1)/2; i++)
        {   f[i]=wdw[i]*cons1*sin(x=d1*((double)i-alp))/x;
            if(i/2*2 != i)  f[i] *= -1.0;
            f[i] +=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
        }
        /* -------------- last half  of the calculation --------------- */
        if((double)((n-1)/2) == alp )
        {   f[(n-1)/2] =wdw[(n-1)/2]*2.0*fc1/fs; // nが奇数の場合
            f[(n-1)/2]+=wdw[(n-1)/2]*2.0*fc/fs;
            for(i=(n+1)/2; i<n; i++)
            {   f[i]=wdw[i]*cons1*sin(x=d1*((double)i-alp))/x;
                if(i/2*2 != i)  f[i] *= -1.0;
                f[i] +=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
            }
        }
        else
        {   for(i=(n-1)/2; i<n; i++)
            {   f[i] =wdw[i]*cons1*sin(x=d1*((double)i-alp))/x;
                if(i/2*2 != i)  f[i] *= -1.0;
                f[i] +=wdw[i]*cons*sin(x=d*((double)i-alp))/x;
            }
        }
    }
    return;
}
/* --------------------------------------------- */
void hamwdw1(double wdw[],int n)
{   int i;
    double d,pi=3.14159265358979323846264338327950288419716939;
    if(n>0)
    {   d=2.0*pi/(double)(n-1);
        for(i=0; i<n; i++) wdw[i]=0.54-0.46*cos(d*i);
    }
}