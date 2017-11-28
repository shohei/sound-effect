#include <stdio.h>
#include <stdlib.h>
#include <wavlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
//static char fnI[]={"Karplus.demo.wav"}; //���y���t�@�C����
//static char fnO[]={"sounddemo.wav"};    //�o�͊y���t�@�C����

    char *fnI,*fnO;
    wav_head bufhead;
    short *ix;
    double *ham1,*filtr,*x,*y;
    double delaysec;
    int i,j,jj,n,funcN,fr,read,reminder,id,ie;
    int n_data,ls;
    int d; //�x���T���v����
    double sum;
    int nfilter,fc,fc0,fc1,fs;
    double a,b;

    void FIRfilt(short ix[],double x[],double filtr[],
                 int nfilter,int ls,int n_data);
    void lpfdsn(double fc,double fs,int n,double wdw[],double f[]);
    void hpfdsn(double fc,double fs,int n,double wdw[],double f[]);
    void bpfdsn(double fleft,double fright,double fs,
                int n,double wdw[],double f[]);
    void befdsn(double fleft,double fright,double fs,
                int n,double wdw[],double f[]);
    void FIRcomb(short ix[],double x[],int d,
                 int ls,int n_data,int id);
    void IIRcomb(short ix[],double x[],double y[],double a,double b,
                 int d,int ls,int n_data,int id);
    void Allpass(short ix[],double x[],double y[],double a,
                 int d,int ls,int n_data);
    void hamwdw1(double wdw[],int n);

    if(argc != 4) {
        printf("\nusage: filt_catalog1 func#  I_filename O_filename\n");
        printf(" func # | 0: FIR LPF, 1: FIR HPF, 2: FIR BPF, 3: FIR BEF\n");
        printf("        | 4: FIR Comb add, 5: FIR Comb subt,\n");
        printf("  I_filename   : Input  sound file name\n");
        printf("  O_filename   : Output sound file name\n");
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

    ls= 8192; // 1��ɏ�������o�b�t�@��
    fs=bufhead.fs;
    nfilter=201;
    fc=6000;
    fc0=2000,fc1=4000;

    ham1=calloc(nfilter,sizeof(double));
    filtr=calloc(nfilter,sizeof(double));
    x=calloc(n_data,sizeof(double));  // ls+dmax�ł悢��dmax�֘A�̏������ȗ����Œ��l
    y=calloc(n_data,sizeof(double));  //�@����
    ix=bufhead.data; //�t�@�C���œǂݍ��񂾃A�h���X��ix�Ɠǂݕς���B���̂̕֋X��B

    hamwdw1(ham1,nfilter);
    switch(funcN) {
    case 0:  // FIR LPF
        lpfdsn((double)fc,(double)fs,nfilter,ham1,filtr);
        FIRfilt(ix,x,filtr,nfilter,ls,n_data); // FIR�t�B���^����
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
    case 4:  // FIR ���` filter�@���Z y(n)=x(n)+x(n-D)
        delaysec = 0.00015;  // �x�����ԁks�l
        // ��Ffs = 44100, delaysec = 0.025 �� d=fs*delaysec; 1102.5
        //     �� 1103�T���v��
        d = (int)((double)fs*delaysec+0.5);
        FIRcomb(ix,x,d,ls,n_data,1);
        break;
    case 5:  // FIR ���`filter �@���Z y(n)=x(n)-x(n-D)
        delaysec=0.00015;  //�b
        d=(int)((double)fs*delaysec+0.5);
        FIRcomb(ix,x,d,ls,n_data,-1);
        break;
    case 6:  // IIR ���`filter �@���Z y(n)=a*x(n) + b*y(n-D)
        a=0.5;
        b=0.8;
        delaysec=0.00015;  //�b
        d=(int)((double)fs*delaysec+0.5);
        IIRcomb(ix,x,y,a,b,d,ls,n_data,1);
        break;
    case 7:   // IIR ���`filter�@�@���Z y(n)=a*x(n) + b*y(n-D)
        delaysec=0.00015; // �x������[�b]
        // ��@fs=44100, delaysec=0.025���@d=fs*delaysec; 1102.5 �� 1103�T���v��
        d=(int)((double)fs*delaysec+0.5);
        a=0.5;
        b= 0.8;
        IIRcomb(ix,x,y,a,b,d,ls,n_data,-1);
        break;
    case 8:   // IIR all pass filter  y(n)=(-g)*x(n) + x(n-D)+  g*x(n-D)
        delaysec=0.010; //�b
        d=(int)((double)fs*delaysec+0.5);
        a=0.7;
        Allpass(ix,x,y,a,d,ls,n_data);
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
{   int i,fr,j,jj,read,reminder,n;
    double sum;

    for(i=0; i<nfilter; i++) {
        x[i]=0.0;
    }
    for(fr=jj=0 ; ; fr++ ) {
        read=fr*ls;
        reminder=n_data-read;
        n=ls;
        if(reminder<ls) {
            n=reminder;
        }
        if(n==0)break;
        // �t�@�C������̓ǂݍ��݂̏ꍇ�͂����ɑ}��
        // if((n=fread( (char *)ix,2,ls,input))<ls)�ŏI����

        for(i=0; i<n; i++) {
            x[i+nfilter]=(double)ix[read+i];
        }
        if(n!=ls) {
            for(i=0; i<ls-n; i++) {
                x[i+n+nfilter]=0.0;
            }
        }

        for(i=0; i<n; i++) { // �t�B���^�����O����
            sum=0.0;                     // ��ݍ���
            for(j=0; j<nfilter; j++)sum += filtr[j]*x[i+(nfilter-1)-j];
            ix[jj++]=(short)sum;
        }
        // �t�@�C���ɏo�͂���ꍇ�͂����ɑ}��
        // fwrite((char *)ix,2,n,output);�@�Ȃ�
        //�Ō��nfilter�����̌v�Z�ɂ��p���邽�߁A�R�s�[
        for(i=0; i<nfilter; i++) x[i]=x[ls+i];
        if (reminder<ls) break;
    }
    return;
}
/* --------------------------------------------------------------- */
void FIRcomb(short ix[],double x[],int d,
             int ls,int n_data,int id)
{   int i,fr,j,jj,read,reminder,n;
    double sum;
// d: delay in number of samples
    //id: 1: additive comb filtering  -1: subtractive comb filtering
//     0: no action
    for(i=0; i<d; i++) {
        x[i]=0.0;
    }
    for(fr=jj=0 ; ; fr++ ) {
        read =fr*ls;
        reminder=n_data-read;
        n=ls;
        if(reminder<ls) {
            n=reminder;
        }
        if(n==0)break;
        // �t�@�C������̓ǂݍ��݂̏ꍇ�͂����ɑ}��
        // if((n=fread( (char *)ix,2,ls,input))<ls)�ŏI����

        for(i=0; i<n; i++) {
            x[i+d]=(double)ix[read+i];
        }
        for(i=0; i<n; i++) { // �t�B���^�����O����
            ix[jj++]=(short) (x[i]+ (id)*x[i+d]);
        }
        // �t�@�C���ɏo�͂���ꍇ�͂����ɑ}��
        // fwrite((char *)ix,2,n,output);�@�Ȃ�
        //�Ō��d�����̌v�Z�ɂ��p���邽�߁A�R�s�[
        for(i=0; i<d; i++) x[i]=x[ls+i];
        if (reminder<ls) break;
    }
    return;
}

/* --------------------------------------------------------------- */
void IIRcomb(short ix[],double x[],double y[],double a, double b,
             int d,int ls,int n_data,int id)
{   int i,fr,j,jj,read,reminder,n;
    double sum;
// d: delay in number of samples
    //id: 1: additive comb filtering  -1: subtractive comb filtering
//     0: no action
    for(i=0; i<d; i++) {
        y[i]=0.0;
    }
    for(fr=jj=0 ; ; fr++ ) {
        read =fr*ls;
        reminder=n_data-read;
        n=ls;
        if(reminder<ls) {
            n=reminder;
        }
        if(n==0)break;
        // �t�@�C������̓ǂݍ��݂̏ꍇ�͂����ɑ}��
        // if((n=fread( (char *)ix,2,ls,input))<ls)�ŏI����

        for(i=0; i<n; i++) {
            x[i]=(double)ix[read+i];
        }
        for(i=0; i<n; i++) { // �t�B���^�����O����
            ix[jj++]=(short)(y[(d+i)%ls]= a*x[i]+ (id)*b*y[i]);
        }
        // �t�@�C���ɏo�͂���ꍇ�͂����ɑ}��
        // fwrite((char *)ix,2,n,output);�@�Ȃ�
        if (reminder<ls) break;
    }
    return;
}
/* --------------------------------------------------------------- */
void Allpass(short ix[],double x[],double y[],double a,
             int d,int ls,int n_data)
{   int i,fr,j,jj,read,reminder,n;
    double sum;
// d: delay in number of samples

    for(i=0; i<d; i++) {
        x[i]=y[i]=0.0;
    }
    for(fr=jj=0 ; ; fr++ ) {
        read =fr*ls;
        reminder=n_data-read;
        n=ls;
        if(reminder<ls) {
            n=reminder;
        }
        if(n==0)break;
        // �t�@�C������̓ǂݍ��݂̏ꍇ�͂����ɑ}��
        // if((n=fread( (char *)ix,2,ls,input))<ls)�ŏI����

        for(i=0; i<n; i++) {
            x[i+d]=(double)ix[read+i];
        }
        for(i=0; i<n; i++) { // �t�B���^�����O���� x[i+d]�����݂�x (������x(n))
            ix[jj++]=(short)(y[(d+i)%ls]= x[i] - a*x[i+d]+ a*y[i]);
        }
        // �t�@�C���ɏo�͂���ꍇ�͂����ɑ}��
        // fwrite((char *)ix,2,n,output);�@�Ȃ�
        //�Ō��d�����̌v�Z�ɂ��p���邽�߁A�R�s�[
        for(i=0; i<d; i++) x[i]=x[ls+i];
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
    /*  cons= w0/pi (since w0 = pi*fc/fs*2 )        */
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
    /*  cons= w0/pi (since w0 = pi*fc/fs*2 )        */
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
        {   f[(n-1)/2]=wdw[(n-1)/2]*2.0*fc/fs; /* this path is when n is kisuu */
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
    /*  cons= w0/pi (since w0 = pi*fc/fs*2 )        */
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
        /* conversion to band pass filter  */
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
    /*  cons= w0/pi (since w0 = pi*fc/fs*2 )        */
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
        {   f[(n-1)/2] =wdw[(n-1)/2]*2.0*fc1/fs; /* this path is when n is kisuu */
            f[(n-1)/2]+=wdw[(n-1)/2]*2.0*fc/fs; /* this path is when n is kisuu */
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
