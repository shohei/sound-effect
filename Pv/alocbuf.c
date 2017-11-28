/* ---------------------------------------------------------
   alocbuf.c
   Description:

              coded by Naotoshi Osaka, 12/13/1999
   --------------------------------------------------------- */
#include <malloc.h>
void *alocbuf(int unit,int nsize)
{
    return((void *)malloc(unit*nsize));
}
