#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wavlib.h"
int wavwrite(wav_head *bufhead, short Nch,long fs, int bit,
             int n_data,char *fn,int id)
{
// id: 1 wav format, -1 raw format
  static char chbuf0[4] = "RIFF";
  static char chbuf1[4] = "WAVE";
  static char chbuf2[4] = "fmt ";
  static char chbuf3[4] = "data";
  int i,ie,header_size;
  FILE *fp;

  header_size = 44; // no extension for linear PCM case
  for(i=0; i<4; i++)bufhead->RIFF[i]      = chbuf0[i];
  bufhead->riff_size = (header_size - 8) + n_data*(bit/8);
  for(i=0; i<4; i++)bufhead->riff_kind[i] = chbuf1[i];
  for(i=0; i<4; i++)bufhead->fmt[i]       = chbuf2[i];
  bufhead->fmt_chnk = 16;
  bufhead->fmt_id = 1;
  bufhead->Nch = Nch;
  bufhead->fs = fs;

  bufhead->dts = bufhead->fs*(bit/8)*bufhead->Nch;
  bufhead->bl_size = (bit/8)*bufhead->Nch;
  bufhead->dts = bufhead->fs*bufhead->bl_size;
  bufhead->bit = (short)bit;

  for(i=0; i<4; i++)bufhead->data_chnk[i] = chbuf3[i];
  bufhead->Nbyte = n_data*(bit/8);

  fp = fopen(fn, "wb");
  if(id==1) {
    fwrite(bufhead, (header_size - 8), 1, fp);
    fwrite(bufhead->data_chnk,8,1,fp);
  }
  ie = fwrite(bufhead->data, sizeof(short), n_data, fp);
  fclose(fp);
  return ie;
}
