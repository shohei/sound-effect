#include <stdio.h>
#include <stdlib.h>
#include "wavlib.h"
int wavread(wav_head *bufhead, char *fn)
{
  FILE *fp;
  int ie,i;
  fp = fopen(fn,"rb");
  fread(bufhead->RIFF,1,4,fp);
  fread(&bufhead->riff_size,4,1,fp);

  fread(bufhead->riff_kind,1,4,fp);
  fread(bufhead->fmt,1,4,fp);       // fmt chunk
  fread(&bufhead->fmt_chnk,4,1,fp); // Nbytes of fmt chunk
  fread(&bufhead->fmt_id,2,1,fp);
  fread(&bufhead->Nch,2,1,fp);
  fread(&bufhead->fs,4,1,fp);
  fread(&bufhead->dts,4,1,fp);
  fread(&bufhead->bl_size,2,1,fp);
  fread(&bufhead->bit,2,1,fp);

  if (bufhead->fmt_chnk !=16) {            // extension chunk
    fread(&bufhead->fmt_ext_size, 2,1,fp); // Nbytes for extension
    bufhead->cbuf = calloc(bufhead->fmt_ext_size, sizeof(char));
    fread(bufhead->cbuf,1,bufhead->fmt_ext_size,fp);
  }
  fread(bufhead->data_chnk,1,4,fp); // data chunk
  fread(&bufhead->Nbyte,4,1,fp);
  bufhead->data = calloc(bufhead->Nbyte,sizeof(short));
  if((ie=fread(bufhead->data,1,bufhead->Nbyte,fp)) < bufhead->Nbyte)
    return 1;
  fclose(fp);
  return ie;
}
