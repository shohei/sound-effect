#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "hamwdw1.c"
#define N 1440

int main(void)
{
  FILE *fdi,*fdo; // �t�@�C���̒�`
  const int n = N, ns = n/4, nt = 3*ns;
  int i,j,ist,Ndata,nresidue;
  double xbuf[N],ham[N],wbuf[N];
  short iwbuf[N],ixbuf[N],izbuf[N];
  extern void hamwdw1(double *wdw,int n);

  fdi = fopen("wave001","r+b");
  fdo = fopen("data1","w+b");
  hamwdw1(ham,n); // hamming window
  if((double)(nresidue=n/ns)!=((double)n/(double)ns)) nresidue++;

  for(i=ist=0; ; i++) {
    // 1�t���[�����M��N�̂����C�ŏ���nt����̓o�b�t�@ixbuf�֊i�[
    if(i==0) {
      Ndata = fread(ixbuf,sizeof(short),nt,fdi); // ����̂݃f�B�X�N
                                                 // �t�@�C������
      if(Ndata < nt) {
        break; // �������x�̒Z���ԉ�����z�肵�Ă��炸�C
               // �����͎����I�Ɏ��s����Ȃ�
      }
    } // end of if(i==0)
    else { // ���̏�������эŌ�̏���
      for(j=0; j<nt; j++) ixbuf[j] = ixbuf[j+ns]; // �V�t�g���
    }
    // 1�t���[�����M��N�̂����C�c���(N-nt)����̓o�b�t�@ixbuf�̑O���ֈړ�
    if(ist==0) {
      Ndata = fread((ixbuf+nt),sizeof(short),ns,fdi);
      if(Ndata < ns) { // �t�@�C���̍ŏI�t���[���̏ꍇ
        for(j=0; j<(ns-Ndata); j++) ixbuf[j+nt+Ndata] = 0;
        ist = 1;       // ist:1 �ŏI�t���[���ɓ��B������Ԃ���̃J�E���g
      }
    } else {
      ist++;
      for(j=0; j<ns; j++) ixbuf[j+nt] = 0;
    }
// �����F
    for(j=0; j<n; j++) {
      wbuf[j] = ham[j]*(double)ixbuf[j]; // ���ƐM���Ƃ̐Ϙa���Z
      izbuf[j] = (short)wbuf[j];
      if(j<nt)iwbuf[j] = izbuf[j] + iwbuf[j+ns];
      else    iwbuf[j] = izbuf[j];
    }
    if((Ndata=fwrite(iwbuf,sizeof(short),ns,fdo))<ns) 
      return(1); // �t�@�C���ւ̏�������
    if(ist==nresidue) break;
  }
  fclose(fdi);
  fclose(fdo);
  return(0);
}
