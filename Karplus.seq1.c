#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "wavlib.h"

int main(int argc, char *argv[])
{
  int n_data,n_dataAll,fmt_id,i,j;
  double f,f0,fs,amp, *buffer,*bufferMix;
  short *ibuf;
  char *fn;
  int irandmax,ipos;
  wav_head bufhead;
  int wavt_len,taper_len;
  double maxAmp = 20000.; // �ő�U��
  double taper_ms,taper_mslast,tempo,maxval,minval,meanval,sum;
  // ���ʃf�[�^
  int nVoice = 5; // ������
  int transpose[] = {0,0,-1,-1,-2}; // �e�����̉����̃V�t�g��
                                    // �i�I�N�^�[�u���j
  int nNote[] = {15,8,8,5,4};       // �e�����̉�����

  int marginPt = 10000; // ���̃o�b�t�@���ɏ�����Ƃ����������
  double voiceAmp[] = {1.5,1.,0.4,0.4,0.5}; // �e�����̑��Ή���
  int voice[][15] = {   // �e�����̉���
    4,7,9,4,2,7,0,5,4,-1,-3,2,7,4,14,
    0,0,-2,-4,-5,-7,-1,5,0,0,0,0,0,0,0,
    7,6,4,2,1,2,7,12,0,0,0,0,0,0,0,
    0,-7,-3,0,9,0,0,0,0,0,0,0,0,0,0,
    0,-2,-3,2,0,0,0,0,0,0,0,0,0,0,0
  };
  double rhythm[][15] = {  // �e�����̉���(���Y��)
    0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,3.,
    1.,1.,1.,1.,1.,1.,1.,3.,0.,0.,0.,0.,0.,0.,0.,
    1.,1.,1.,1.,1.,1.,1.,3.,0.,0.,0.,0.,0.,0.,0.,
    3.,1.,1.,2.,3.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
    3.,1.,1.,5.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
  };

  int KarplusStrong(int wavt_len, short ibuf[], double buffer[],
                    double amp, int n_data, int taper_len,
                    int irandmax);
  /* ------------------------------------------------------------ */
  if(argc != 6) {
    printf("\nusage: Karplus.seq1 fmt amp tempo fs filename\n");
    printf("   fmt        : wav/raw= 1/-1\n");
    printf("   amp        : amplitude\n");
    printf("   tempo      : tempo (beats per min)\n");
    printf("   fs         : sampling frequency in Hz (real)\n");
    printf("   filename   : file name of sound\n");
    printf(" note: if fmt is given in negative,\n");
    printf("        output file is 'raw' format. ");
    printf("Otherwize 'wav' format\n");
    return 0;
  }
  taper_ms = 0.15;
  taper_mslast = 0.5;
  fmt_id = atoi(argv[1]);
  irandmax = RAND_MAX;
  fn = calloc(100,sizeof(char));

  f0 = 220.5*1.5;
  amp   = atof(argv[2]);
  tempo = atof(argv[3]);
  fs    = atof(argv[4]);
  sscanf(argv[5],"%s",fn);

  // �S�f�[�^���Z�o�Dtempo���l��
  for(j=n_dataAll=0; j<nNote[0]; j++) 
    n_dataAll += (int)(60./tempo*rhythm[0][j]*fs);
  n_dataAll += marginPt;
  buffer = calloc(n_dataAll,sizeof(double));
  bufferMix = calloc(n_dataAll,sizeof(double));
  ibuf = bufhead.data = calloc(n_dataAll,sizeof(short));

  for(i=0; i<nVoice; i++) { // �����̃��[�v�DbufferMix�ɍ�������~����
    for(j=ipos=0; j<nNote[i]; j++) { // �����̃��[�v�Dbuffer��1������
                                     // �M���݂̂��i�[
      f = pow(2.,(double)transpose[i])*f0
          *pow(2.,(double)voice[i][j]/12. );
      wavt_len = (int)(fs/f);
      n_data = (int)(60./tempo*rhythm[i][j]*fs);
      taper_len = taper_ms*fs;
      srand((unsigned)time(NULL));
      KarplusStrong(wavt_len,ibuf+ipos,buffer+ipos,amp*voiceAmp[i],
                    n_data,taper_len,irandmax);
      ipos += n_data;
    }
    if(i==0) {
      for(j=0; j<n_dataAll; j++) bufferMix[j] = buffer[j];
    } else {
      for(j=0; j<n_dataAll; j++) bufferMix[j] += buffer[j];
    }
  }
  // �M���̒����D������(���M���̕��ϒl)�Z�o�D���������
  // ������̍ő�/�ő�l�Z�o
  sum = 0.;
  for(j=0; j<n_dataAll; j++) sum += bufferMix[j];
  meanval = sum/(double)n_dataAll;
  maxval = minval = 0.;
  for(j=0; j<n_dataAll; j++) {
    bufferMix[j] -= meanval;
    if(bufferMix[j]>maxval) maxval = bufferMix[j];
    if(bufferMix[j]<minval) minval = bufferMix[j];
  }
  if(abs(minval)>maxval) maxval = abs(minval);
  // �M���U����maxAmp�ƂȂ�悤����
  // �Ō��fade out��������
  taper_len = taper_mslast*fs;
  for(j=n_dataAll - taper_len; j<n_dataAll; j++)
    buffer[j] *= ((double)(j-n_data+taper_len)
                  *(-1./(double)taper_len) + 1.0);
  for(j=0; j<n_dataAll; j++) 
    ibuf[j] = (short)(bufferMix[j]*maxAmp/maxval);

  wavwrite(&bufhead,1,fs,bits,n_dataAll,fn,fmt_id);

  free(ibuf);
  free(buffer);
  free(bufferMix);
  free(bufhead.data);
  free(fn);
  return 0;
}
/* ------------------------------------------------------------ */
int KarplusStrong(int wavt_len, short ibuf[], double buffer[],
                  double amp, int n_data, int taper_len,
                  int irandmax)
{
  int i;
  for(i=0; i<=wavt_len; i++)  /* generation of white noise */
    ibuf[i] = (short)(buffer[i] = (-amp)+rand()*(2.*amp)
                                  /(irandmax+1.));
  for(i=wavt_len+1; i<n_data; i++)
    ibuf[i] = (short)(buffer[i] = 0.5*(buffer[i-wavt_len]
                                       +buffer[i-wavt_len-1]));
  for(i=n_data - taper_len; i<n_data; i++)
    ibuf[i] = (short)(buffer[i]*((double)(i-n_data+taper_len)
                                 *(-1./(double)taper_len) + 1.0));
  return 0;
}
