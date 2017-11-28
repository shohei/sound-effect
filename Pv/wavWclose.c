/* -----------------------------------------------------------------
 id: 1 ->wav format, otherwize -> raw format
  ------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <wavlib.h>
int wavWclose(wav_head *bufhead,int n_data,FILE *fp,int id)
{
    int header_size;


    if(id==1) {
        header_size=44;
        rewind(fp);
        bufhead->Nbyte = n_data * (bufhead->bit/8);
        bufhead->riff_size=(header_size - 8) +  bufhead->Nbyte;
        fwrite(bufhead, (header_size - 8), 1, fp);
        fwrite(bufhead->data_chnk,8,1,fp);
    }
    fclose(fp);
    printf("\n%-d data were stored\n", n_data);

    return 0;
}

