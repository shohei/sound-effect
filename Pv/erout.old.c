/* ------------------------------------------------------------
   erout
   Description:
    error code output
   input:
    ie: error code
   coded by Naotoshi Osaka     9/30/1993
   ------------------------------------------------------------ */
#include <stdio.h>
void erout(int ie)
{
    static char *em[4]= {"main error(0) nshift  > NSHIFT \n",
                         "main error(1) nwdw  > NWDW \n",
                         "main error(2) nharm0> nhc.\n",
                         "main error(3) nharm > nharmmax.\n"
                        };

    printf("%s\n",em[ie]);
}
