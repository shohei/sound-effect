#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "wavlib.h"

int main(int argc, char *argv[])
{
  int fmt_id,Nformant,i,j,k,l;
  short *ibuf;
  char *fn;
  double pi; /* =3.14159265358979323846 */
  int Nfft,fs,Np,NpAll,Ntaumax,Ntau,tau,irandmax;
  double nsec;
  double **s,*y;
  double Cf10,Cf2,Cf30,Cf4,Cf5;   // ���S���g��
  double BW10,BW2,BW3,BW4,BW5;    // �o���h��
  double A1,A2,A3,A4,A5,maxAmp;   // ���ΐU��(0-1)�ƍő�U��
  double Atk1,Atk2,Atk3,Atk4,Atk5;// ��/B�F�X�J�[�g���C���Ȃ킿�C
                                  // attack time�ks�l
  double Iphase1,Iphase2,Iphase3,Iphase4,Iphase5; // �����ʑ��krad�l
  double f0,vibf,Av;              // �r�u���[�g�֘A
  double vibCf,AvCf,vibBW,AvBW;   // ���S���g���̂�炬�C�o���h���̂�炬
  double Avrand0,Cfrand0,BWrand0; // ��炬�̎G������
  double *Avrand,*Cfrand,*BWrand,*f00,*Cf1,*Cf3,*BW1;
  double a; // 2���t�B���^�ɂ��ȈՂȍ���f�B�G���t�@�V�X�t�B���^
  // �����オ��(attack)�Ɨ���������(release)���ꂼ��̎��萔
  double AtkOval,RelOval,Atkpt,Relpt;
  wav_head bufhead;
  extern int FOF(double * s,double Cf,double BW,double A,
                 double Atk,double Iphase,int fs,int Np);

  if(argc != 2) {
    printf("\nusage: chantS filename\n");
    printf("   filename   : file name of sound\n");
    return 0;
  }
  fn = calloc(100,sizeof(char));
  sscanf(argv[1],"%s",fn);

  pi = M_PI;
  irandmax = RAND_MAX;

  Nfft = 4096;  // FFT����
  fs = 44100;   // �T���v�����O���g���kHz�l
  Np = Nfft;    // �C���p���X�����̃T���v����
  nsec = 2.4;   // �����̑����I�Ȏ��Ԓ��ks�l
  NpAll = (int)((double)fs*nsec); // �����T���v����
  fmt_id = 1;   // �o�̓t�@�C�������D1: wav file, 0: Raw file format
  Nformant = 5; // �t�H���}���g��
  maxAmp = 32768.*pow(10.,-3.01/20.);  // 16bit short�̍ő�l���
                                       // 3dB�Ⴂ�l = 23171

  Cf10=980.; Cf2=2450.; Cf30=3920.; Cf4=7000.; Cf5= 9000.; //�kHz�l
  BW10=500.; BW2=500.; BW3=500.; BW4=500.; BW5=500.; 
                                        //�kHz�l Alpha=BW*pi (FOF��)
  A1=0.6; A2=0.01; A3=0.4; A4=0.1; A5=0.02;  // Amplitude
  Atk1=0.005; Atk2=0.003; Atk3=0.05; Atk4=0.05; Atk5=0.05; // ��/B:
                               // �X�J�[�g��, ���Ȃ킿, attack time�ks�l
  Iphase1=0; Iphase2=1/20*pi;  Iphase3=2/20*pi;
  Iphase4=3/20*pi; Iphase5=4/20*pi;  // Initial Phase 
  A1 *=maxAmp;  A2 *=maxAmp; A3 *=maxAmp; A4 *=maxAmp; A5 *=maxAmp;
  f0 = 490;
  Ntaumax = 2000;
  vibf = 6;
  Av = 0.06;  

  vibCf = 7;
  AvCf = 0.02;
  vibBW = 7;
  AvBW = 0.02;

  Avrand0 = 0.02;
  Cfrand0 = 0.5;
  BWrand0 = 0.5;
  srand((unsigned)time(NULL));
  /* --------------------------------------------------------
     �������m�ہ@s[Nformant][Np]��2���� s[0][*]=��s[i][*]
     -------------------------------------------------------- */
  s = calloc(Nformant,sizeof(double *));
  for(j=0; j<=Nformant; j++) s[j]=calloc(Np,sizeof(double));
  y = calloc(NpAll,sizeof(double));
  ibuf = bufhead.data=calloc(NpAll,sizeof(short));

  Avrand = calloc(Ntaumax,sizeof(double));
  Cfrand = calloc(Ntaumax,sizeof(double));
  BWrand = calloc(Ntaumax,sizeof(double));
  f00    = calloc(Ntaumax,sizeof(double));
  Cf1    = calloc(Ntaumax,sizeof(double));
  Cf3    = calloc(Ntaumax,sizeof(double));
  BW1    = calloc(Ntaumax,sizeof(double));
  for(i=0; i<Ntaumax; i++) { /* generation of random parameters */
    Avrand[i] = ((double)rand()/(irandmax+1.)-0.5)*Avrand0;
    Cfrand[i] = ((double)rand()/(irandmax+1.)-0.5)*Cfrand0;
    BWrand[i] = ((double)rand()/(irandmax+1.)-0.5)*BWrand0;
  }
  /* ------ FOF�t�B���^ s[k]�C����яo�͐M�� y[] �̎Z�o -------- */
// j is tau count, while k is position (discrete time) count
  for(j=k=0;; j++) {
    Cf1[j] = (1+AvCf*sin(2*pi*vibCf*k/fs+Cfrand[j]))*Cf10;
    Cf3[j] = (1+AvCf*sin(2*pi*vibCf*k/fs+Cfrand[j]))*Cf30;
    BW1[j] = (1+AvBW*sin(2*pi*vibBW*k/fs+BWrand[j]))*BW10;

    FOF(s[1],Cf1[j],BW1[j],A1,Atk1,Iphase1,fs,Np);
    FOF(s[2],Cf2,BW2,A2,Atk2,Iphase2,fs,Np);
    FOF(s[3],Cf3[j],BW3,A3,Atk3,Iphase3,fs,Np);
    FOF(s[4],Cf4,BW4,A4,Atk4,Iphase4,fs,Np);
    FOF(s[5],Cf5,BW5,A5,Atk5,Iphase5,fs,Np);

    for(l=0; l<Np; l++) 
      s[0][l]=s[1][l]+s[2][l]+s[3][l]+s[4][l]+s[5][l];

    f00[j] = (1+Av*sin(2*pi*vibf*k/fs+Avrand[j]))*f0;
    tau = (int)floor(fs/f00[j]);
    if((k+Np)>NpAll) break;
    for(l=0;l<Np;l++) y[k+l] = y[k+l]+s[0][l];// y�̃I�[�o�[���b�v�A�h
    k = k+tau;
  }
  Ntau = j;
  NpAll = k;
  /* -------------- Post processing: De-emphasis------------- */
  a = 0.98;
  y[0] /= 2.;
  for (j=1; j<NpAll; j++) y[j] = (a*y[j-1]+y[j])/2.;
  // �E�ӂ�y[j]�͗��_�I�ɂ͓���x[j]
  /* --------- Overall envelope: Attack and release --------- */
  AtkOval = 0.1;  // second
  RelOval = 0.1;  // second
  Atkpt = floor((double)fs*AtkOval);
  Relpt = floor((double)fs*RelOval);
  for (j=0; j<Atkpt; j++) y[j]=(double)j/(double)Atkpt*y[j];
  for (j=0; j<Relpt; j++) 
    y[NpAll-1-j]=(double)j/(double)Atkpt*y[NpAll-1-j];
  for (j=0; j<NpAll; j++) ibuf[j]=(short)y[j];

  wavwrite(&bufhead,1,fs,bits,NpAll,fn,fmt_id);

  /*----------- �������̉�� --------- */
  free(y);
  free(s);
  free(bufhead.data);
  free(fn);
  free(Avrand);
  free(Cfrand);
  free(BWrand);
  free(f00);
  free(Cf1);
  free(Cf3);
  free(BW1);
  return 0;
}