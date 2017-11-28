#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "hamwdw1.c"
#define N 1440

int main(void)
{
  FILE *fdi,*fdo; // ファイルの定義
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
    // 1フレーム内信号N個のうち，最初のnt個を入力バッファixbufへ格納
    if(i==0) {
      Ndata = fread(ixbuf,sizeof(short),nt,fdi); // 初回のみディスク
                                                 // ファイルから
      if(Ndata < nt) {
        break; // 窓長程度の短時間音声を想定しておらず，
               // ここは実質的に実行されない
      }
    } // end of if(i==0)
    else { // 定常の処理および最後の処理
      for(j=0; j<nt; j++) ixbuf[j] = ixbuf[j+ns]; // シフト作業
    }
    // 1フレーム内信号Nのうち，残りの(N-nt)個を入力バッファixbufの前方へ移動
    if(ist==0) {
      Ndata = fread((ixbuf+nt),sizeof(short),ns,fdi);
      if(Ndata < ns) { // ファイルの最終フレームの場合
        for(j=0; j<(ns-Ndata); j++) ixbuf[j+nt+Ndata] = 0;
        ist = 1;       // ist:1 最終フレームに到達した状態からのカウント
      }
    } else {
      ist++;
      for(j=0; j<ns; j++) ixbuf[j+nt] = 0;
    }
// 処理：
    for(j=0; j<n; j++) {
      wbuf[j] = ham[j]*(double)ixbuf[j]; // 窓と信号との積和演算
      izbuf[j] = (short)wbuf[j];
      if(j<nt)iwbuf[j] = izbuf[j] + iwbuf[j+ns];
      else    iwbuf[j] = izbuf[j];
    }
    if((Ndata=fwrite(iwbuf,sizeof(short),ns,fdo))<ns) 
      return(1); // ファイルへの書き込み
    if(ist==nresidue) break;
  }
  fclose(fdi);
  fclose(fdo);
  return(0);
}
