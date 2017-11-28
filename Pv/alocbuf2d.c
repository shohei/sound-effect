#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

/* ------------------------------------------------------
     allocation of  buf[ndim1][ndim0]
calling sequence
  double **buf;
  alocbuf2d_d((double ***)&buf,ndim1,ndim0);
  buf[ndim1][nidm0] is available.

        coded by Naotoshi osaka, 2/7/'00
   ------------------------------------------------------ */
extern void *alocbuf(int unit,int nsize);
void alocbuf2d_d(double ***dbuf,int ndim1,int ndim0)
{   int i;
    *dbuf = (double **)alocbuf(sizeof(double *),ndim1);
    for(i=0; i<ndim1; i++) {
        (*dbuf)[i]=(double *)alocbuf(sizeof(double),ndim0);
    }
}
void alocbuf2d_i(int ***ibuf,int ndim1,int ndim0)
{   int i;
    *ibuf = (int **)alocbuf(sizeof(int *),ndim1);
    for(i=0; i<ndim1; i++)(*ibuf)[i]=(int *)alocbuf(sizeof(int),ndim0);
}
void alocbuf2d_s(short ***sbuf,int ndim1,int ndim0)
{   int i;
    *sbuf = (short **)alocbuf(sizeof(short *),ndim1);
    for(i=0; i<ndim1; i++)(*sbuf)[i]=(short *)alocbuf(sizeof(short),ndim0);
}
void alocbuf2d_c(char ***cbuf,int ndim1,int ndim0)
{   int i;
    *cbuf = (char **)alocbuf(sizeof(char *),ndim1);
    for(i=0; i<ndim1; i++)(*cbuf)[i]=(char *)alocbuf(sizeof(char),ndim0);
}
/* ------- free double's buffer -------- */
void freemem_d(double ***buf,int ndim1)
{   int i;
    for(i=0; i<ndim1; i++) {
        if((*buf)[i]!=NULL) {
            free((*buf)[i]);
            (*buf)[i]=NULL;
        }
    }
    if(*buf!=NULL) {
        free(*buf);
        *buf=NULL;
    }
}
/* ------- free double's buffer -------- */
void freemem_i(int ***ibuf,int ndim1)
{   int i;
    for(i=0; i<ndim1; i++) {
        if((*ibuf)[i]!=NULL) {
            free((*ibuf)[i]);
            (*ibuf)[i]=NULL;
        }
    }
    if(*ibuf!=NULL) {
        free(*ibuf);
        *ibuf=NULL;
    }
}
/* ------- free double's buffer -------- */
void freemem_s(short ***sbuf,int ndim1)
{   int i;
    for(i=0; i<ndim1; i++) {
        if((*sbuf)[i]!=NULL) {
            free((*sbuf)[i]);
            (*sbuf)[i]=NULL;
        }
    }
    if(*sbuf!=NULL) {
        free(*sbuf);
        *sbuf=NULL;
    }
}
/* ------- free double's buffer -------- */
void freemem_c(char ***cbuf,int ndim1)
{   int i;
    for(i=0; i<ndim1; i++) {
        if((*cbuf)[i]!=NULL) {
            free((*cbuf)[i]);
            (*cbuf)[i]=NULL;
        }
    }
    if(*cbuf!=NULL) {
        free(*cbuf);
        *cbuf=NULL;
    }
}
/* ======================================================= */
/* void main()
{ double **buf;  int **ibuf;  short **sbuf;char **cbuf;
 int i,j;
  extern void alocbuf2d_d(double ***dbuf,int nd1,int nd0);
  extern void alocbuf2d_i(int    ***ibuf,int nd1,int nd0);
  extern void alocbuf2d_s(short  ***sbuf,int nd1,int nd0);
  extern void alocbuf2d_c(char   ***cbuf,int nd1,int nd0);
  extern void   freemem_d(double ***buf,int ndim1);
  extern void   freemem_i(int    ***ibuf,int ndim1);
  extern void   freemem_s(short  ***sbuf,int ndim1);
  extern void   freemem_c(char   ***cbuf,int ndim1);

  int ndim1=5; int ndim0=10;
    printf(" &buf=%d\n",(int)&buf); */
/* ----- double buf test ------------ */
/*   alocbuf2d_d((double ***)&buf,ndim1,ndim0);
  for(i=0;i<ndim1;i++){
    for(j=0;j<ndim0;j++){
      buf[i][j]= i*j;
      printf("buf[%d][%d]=%lf\n",i,j,buf[i][j]);
    }
  }
freemem_d((double ***)&buf,ndim1);
*/
/* ----- int buf test ------------ */
/* alocbuf2d_i((int ***)&ibuf,ndim1,ndim0);
  for(i=0;i<ndim1;i++){
    for(j=0;j<ndim0;j++){
      ibuf[i][j]= i+j;
      printf("buf[%d][%d]=%d\n",i,j,ibuf[i][j]);
    }
  }
freemem_i((int ***)&ibuf,ndim1); */
/* ----- short buf test ------------ */
/*  alocbuf2d_s((short ***)&sbuf,ndim1,ndim0);
  for(i=0;i<ndim1;i++){
    for(j=0;j<ndim0;j++){
      sbuf[i][j]= i*j;
      printf("buf[%d][%d]=%d\n",i,j,sbuf[i][j]);
    }
  }
freemem_s((short ***)&sbuf,ndim1); */
/* ---- char buf test ------------- */
/* ndim0=100;
  alocbuf2d_c((char ***)&cbuf,ndim1,ndim0);
  for(i=0;i<ndim1;i++){
      strcpy(cbuf[i],"This is a test");
      printf("cbuf[%d]=%s\n",i,cbuf[i]);

  }
freemem_c((char ***)&cbuf,ndim1);
} */
