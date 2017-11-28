#include <stdio.h>
#include <stdlib.h>
#include <wavlib.h>
/* ver. 1.0 */
int main(int argc, char *argv[])
{
  char *fnI,*fnO;
  wav_head bufhead;
  int i,funcN,n_data,ie;
  int nonLinAmp(short ibuf[],int n_data,short ibufout[],
                double amptbl[],int nbuf,double magicN);
  int nbuf = 512;
  double thd,thd0=0.60,maxamp,magicN,a,b,c,amptbl[513];

  if(argc != 4) {
    printf("\nusage: %s func# I_filename O_filename\n",argv[0]);
    printf(" func#: 0: nonlinear amp\n");
    printf("  I_filename   : Input  sound file name\n");
    printf("  O_filename   : Output sound file name\n");
    return 0;
  }
  fnI = calloc(100,sizeof(char));
  fnO = calloc(100,sizeof(char));
  funcN = atoi(argv[1]);
  sscanf(argv[2],"%s",fnI);
  sscanf(argv[3],"%s",fnO);

  wavread(&bufhead,fnI);
  n_data = bufhead.Nbyte/bufhead.bl_size;
  switch(funcN) {
  case 0: // ”ñüŒ`ƒAƒ“ƒv
    printf("magicN?");
    scanf("%lf",&magicN);
    printf("magicN=%lf\n",magicN);
    thd = (double)nbuf*thd0;
    a = 1./(2.0*(thd-(double)nbuf));
    b = (-2.0)*a*(double)nbuf;
    c = (-a)*thd*thd + (1.0-b)*thd;

    for (i=nbuf-1; i>=0; i--) {
      if(i<=(int)thd) {
        amptbl[i] = (double)i;
      } else {
        amptbl[i] = a*(double)i*(double)i + b*(double)i + c;
        if(i==(nbuf-1)) {
          maxamp = amptbl[i];
        }
      }
      amptbl[i] *= (double)(nbuf-1)/((double)nbuf*maxamp);
    }
    amptbl[nbuf] = 1.0; // put 1 in extra address
    ie = nonLinAmp(bufhead.data,n_data,bufhead.data,amptbl,nbuf,
                   magicN);
    break;
  }
  wavwrite(&bufhead, bufhead.Nch, bufhead.fs,
           bufhead.bit, bufhead.Nbyte/bufhead.bl_size, fnO, 1);
  return 0;
}
/* -------------------------------------------------------------- */
int nonLinAmp(short ibuf[],int n_data,short ibufout[],
              double amptbl[],int nbuf,double magicN)
{
  int i,isgn,idx,limamp;
  double w,w0,temp;
  //      w: nbuf‚É‘Î‚·‚éŒ»ÝˆÊ’u‚Ì”ä—¦
  //     w0: idx‚Æidx+1‚ÌŠÔŠu‚É‘Î‚·‚éidx‚ÆŒ»ÝˆÊ’u‚ÌŠÔŠu‚Ì”ä—¦
  // ambtbl: 0-1‚É³‹K‰»‚³‚ê‚½‘•“Á«•\
  // magicN: ŽÀ”‰‰ŽZŽž‚Ì‘•ŒW”DI—¹Œã‚É‚ÍœŽZ‚ð‚µ‚ÄŒ³‚É–ß‚·
  // ”ñüŒ`‚Ì“K—p—Ìˆæ‚ð§Œä
  limamp = 32700;
  for (i=0; i<n_data; i++) {
    if(ibuf[i]==0) ibufout[i] = 0;
    else if(abs(ibuf[i])*magicN>limamp)
      ibufout[i] = (isgn=abs(ibuf[i])/ibuf[i])*limamp/(int)magicN;
    else {
      isgn = abs(ibuf[i])/ibuf[i];
      w = (double)abs(ibuf[i])*magicN/(double)limamp;
      idx = (int)(w*(double)nbuf);
      w0 = w*(double)nbuf - (double)idx;

      ibufout[i] = isgn* (short)(temp=(double)limamp*((1.-w0)
                                 *amptbl[idx]+w0*amptbl[idx+1])
                                 /magicN);
    }
  }
  return 0;
}
