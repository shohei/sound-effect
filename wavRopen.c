#include <stdio.h>
#include <stdlib.h>
#include <wavlib.h>
FILE* wavRopen(wav_head *bufhead, char *fn)   //)
{
    int ie,i;
    FILE *fp;
    fp=fopen(fn,"rb");

    fread(bufhead->RIFF,1,4,fp);
    fread(&bufhead->riff_size,4,1,fp);

    fread(bufhead->riff_kind,1,4,fp);
    fread(bufhead->fmt,1,4,fp);  // fmt chunk
    fread(&bufhead->fmt_chnk,4,1,fp); // Nbytes of fmt chunk
    fread(&bufhead->fmt_id,2,1,fp);
    fread(&bufhead->Nch,2,1,fp);
    fread(&bufhead->fs,4,1,fp);
    fread(&bufhead->dts,4,1,fp);
    fread(&bufhead->bl_size,2,1,fp);
    fread(&bufhead->bit,2,1,fp);
    if (bufhead->fmt_chnk !=16) { // extension chunk
        fread(&bufhead->fmt_ext_size, 2,1,fp); // Nbytes for extension
        bufhead->cbuf=calloc(bufhead->fmt_ext_size, sizeof(char));
        fread(bufhead->cbuf,1,bufhead->fmt_ext_size,fp);
    }
    fread(bufhead->data_chnk,1,4,fp); // data chunk
    fread(&bufhead->Nbyte,4,1,fp);
    return fp;
}