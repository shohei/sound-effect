#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <wavlib.h>
#include "hanwdw1.c"

int main(int argc, char *argv[])
{
  int nwdw;
  int nshiftA,ntA,nshiftS,ntS;
  int i,j,ie,ist,Ndata,n_data,nresidue;
  double *han,*wbuf,*xbuf,ratio;
  short *iwbuf,*ixbuf,izbuf;
  wav_head bufhead;
  char *fnI,*fnO;

  FILE *fdi,*fdo;

  extern void hanwdw1(double *wdw,int nwdw);
  extern FILE* wavRopen(wav_head *bufhead, char *fn);
  extern FILE* wavWopen(wav_head *bufhead, short Nch,long fs, 
                        int bit, int n_data, char *fn, int id);
  extern int wavWclose(wav_head *bufhead,int n_data,FILE *fp,int id);
  if(argc != 6) {
    printf("\nusage: overlapAdd_granular I_fn  nwdw  nshiftA");
    printf("  ratio(real)  O_fn\n");
    printf("  I_fn   : Input sound file name\n");
    printf("  nwdw   : Number of window samples\n");
    printf("  nshiftA: Number of samples for frame shift");
    printf(" in Analysis\n");
    printf("  ratio  : [nshiftS/nshiftA] in real, where nshiftS ");
    printf("is nshift in Synthesis\n");
    printf("  O_fn   : Output sound file name\n");
    printf("ex: overlapAdd_granular Wave001.wav 1440 360 3.0");
    printf(" data1.wav\n");
    printf("    gives three times as long as original sound.\n");
    return 0;
  }
  fnI = calloc(100,sizeof(char));
  fnO = calloc(100,sizeof(char));
  sscanf(argv[1],"%s",fnI);
  nwdw    = atoi(argv[2]); // ����
  nshiftA = atoi(argv[3]); // ���͗p�t���[���V�t�g// typically nwdw/4;
  ratio   = atof(argv[4]); // �����p�t���[���V�t�g�ƕ��͗p�t���[���V�t�g�̔�
  sscanf(argv[5],"%s",fnO);

  ntA = nwdw-nshiftA;
  nshiftS = (int)((double)nshiftA*ratio); // �����p�t���[���V�t�g
  ntS = nwdw-nshiftS;
  n_data = 0;                             // �����o�̓f�[�^��

  ixbuf = calloc(nwdw,sizeof(short));     // �e��o�b�t�@���蓖��
  if(nshiftS<=nwdw) {
    iwbuf = calloc(nwdw,sizeof(short));
    xbuf  = calloc(nwdw,sizeof(double));  // �������Z�p�o�b�t�@
  } else {
    iwbuf = calloc(nshiftS,sizeof(short));
    xbuf  = calloc(nshiftS,sizeof(double));
  }
  han  = calloc(nwdw,sizeof(double));     // �n�j���O���f�[�^
  wbuf = calloc(nwdw,sizeof(double));

  fdi = wavRopen(&bufhead,fnI);
  fdo = wavWopen(&bufhead, bufhead.Nch, bufhead.fs,bufhead.bit,
                 bufhead.Nbyte/bufhead.bl_size, fnO, 1);

  hanwdw1(han,nwdw); // hanning window
  if((double)(nresidue=nwdw/nshiftA)!=((double)nwdw/(double)nshiftA))
    nresidue++;

  for(i=ist=0; ; i++) {
    // 1�t���[�����M��N�̂����C�ŏ���nt����̓o�b�t�@ixbuf�֊i�[
    if(i==0) {
      Ndata = fread(ixbuf,sizeof(short),ntA,fdi); // ����̂݃f�B�X�N
                                                  // �t�@�C������
      if(Ndata < ntA) {
        break; // �������x�̒Z���ԉ�����z�肵�Ă��炸�C
               // �����͎����I�Ɏ��s����Ȃ�
      }
    } // end of if(i==0)
    else { // ���̏�������эŌ�̏���
      for(j=0; j<ntA; j++)ixbuf[j]=ixbuf[j+nshiftA]; // �V�t�g���
    }
    // 1�t���[�����M��N�̂����C�c���(N-nt)����̓o�b�t�@ixbuf�̑O���ֈړ�
    if(ist==0) {
      Ndata = fread((ixbuf+ntA),sizeof(short),nshiftA,fdi);
      if(Ndata < nshiftA) { // �t�@�C���̍ŏI�t���[���̏ꍇ
        for(j=0; j<(nshiftA-Ndata); j++) ixbuf[j+ntA+Ndata] = 0;
        ist = 1;       // ist:1 �ŏI�t���[���ɓ��B������Ԃ���̃J�E���g
      }
    } else {
      ist++;
      for(j=0; j<nshiftA; j++) ixbuf[j+ntA] = 0;
    }
// �����F
    for(j=0; j<nwdw; j++) {
      wbuf[j] = han[j]*(double)ixbuf[j]; // ���ƐM���Ƃ̐Ϙa���Z
      izbuf = (short)wbuf[j];
      if(j<ntS) {
        iwbuf[j] = izbuf + iwbuf[j+nshiftS];
        xbuf[j] = wbuf[j]+xbuf[j+nshiftS];
      } else {
        iwbuf[j] = izbuf;
        xbuf[j] = wbuf[j];
      }
    }
    if(nshiftS>nwdw) {
      for(j=nwdw; j<nshiftS; j++) {
        iwbuf[j] = 0;
        xbuf[j] = 0.0;
      }
    }
    for(j=0; j<nshiftS; j++) iwbuf[j] = (short)xbuf[j];
    if((Ndata=fwrite(iwbuf,sizeof(short),nshiftS,fdo))<nshiftS)
      return(1); // �t�@�C���ւ̏�������
    n_data += Ndata;
    if(ist==nresidue) break;
  }
  fclose(fdi);
  wavWclose(&bufhead,n_data,fdo,1);
  free(ixbuf);
  free(iwbuf);
  free(han);
  free(wbuf);
  free(xbuf);
  return(0);
}
