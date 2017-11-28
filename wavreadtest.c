#include <stdio.h>
#include <stdlib.h>
#include "wavlib.h"

int main(int argc, char *argv[])
{
  char *fnI,*fnO;
  wav_head bufhead;
  int i;

  if(argc != 3) {
    printf("\nusage: %s I_filename O_filename\n",argv[0]);
    printf("  I_filename   : Input  sound file name\n");
    printf("  O_filename   : Output sound file name\n");
    return 0;
  }
  fnI = calloc(100,sizeof(char));
  fnO = calloc(100,sizeof(char));
  sscanf(argv[1],"%s",fnI);
  sscanf(argv[2],"%s",fnO);

  wavread(&bufhead,fnI);
  printf("riff_size=%d\n",bufhead.riff_size);
  printf("Nch=%d\n",bufhead.Nch);
  printf("fs=%d\n",bufhead.fs);
  printf("bit=%d\n",bufhead.bit);
  printf("n_data=%d\n",bufhead.Nbyte/bufhead.bl_size);
  wavwrite(&bufhead, bufhead.Nch, bufhead.fs,
           bufhead.bit, bufhead.Nbyte/bufhead.bl_size, fnO, 1);
  return 0;
}
