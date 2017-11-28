static int bits=16;
typedef struct {
  char  RIFF[4];
  long  riff_size;
  char  riff_kind[4];
  char  fmt[4];
  long  fmt_chnk;
  short fmt_id;
  short Nch;
  long  fs;
  long  dts;
  short bl_size;
  short bit;
  short fmt_ext_size;
  char  *cbuf;
  char  data_chnk[4];
  long  Nbyte;
  short *data;
} wav_head;
extern int wavread(wav_head *bufhead, char *fn);
extern int wavwrite(wav_head *bufhead, short Nch,
                    long fs, int bit, int n_data,char *fn,int id);
