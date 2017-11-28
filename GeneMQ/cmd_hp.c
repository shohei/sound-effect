/* -----------------------------------------------------------------------
    com_hp.c
     command: ?/hp/he/help

          coded by Naotoshi Osaka   4/30/'00
   ----------------------------------------------------------------------- */
#include <string.h> 
#include <math.h>
#include <stdio.h>

void cmd_hp(int narg,char *cmnd[],char ***arg)
{
    if(narg==1){  
/*   printf("2nd an:no arg; narg=%d \n  nest=%s\n (*arg)[0]=%s\n",
   narg,nest,(*arg)[0]); */
     printf(" ex   exit\n");
     printf(" hp/?/he/help     help\n");
     printf(" ru   run synthesis process from packets\n");
     printf(" ls   list packets\n");
     printf(" ref  reference packet input\n");
     printf(" del  delete tracks\n");
     printf(" pic  pick frames for all track\n");
     printf(" e    change packet parameter (interactive)\n");
     printf(" cv  frame order/track order conversion\n");
     printf(" zed  discard magnitude zero packets \n");
     printf(" syn  synthesize sound \n");
     printf(" mat  freq and magnitude data output for matlab \n");
     printf("  ** for more information, hp [command]. ** \n");
    }
   else {  /* hp with argument */
     /*   printf("2nd an:with arg; narg=%d \n  (*arg)[1]=%s\n",
   narg,(*arg)[1]); */
     if(strcmp((*arg)[1],cmnd[0])==0){  /* ex */
      printf(" exit. (with no argument)\n");
     }
     else if (strcmp((*arg)[1],cmnd[1])==0 ||
              strcmp((*arg)[1],cmnd[2])==0 ||
              strcmp((*arg)[1],cmnd[3])==0 ||
              strcmp((*arg)[1],cmnd[4])==0){ /* hp */
      printf(" help: with 1 argument to specify command\n");
     }
     else if (strcmp((*arg)[1],cmnd[5])==0){ /* ru */
      printf(" ru: update packet file (.pkt)\n");
     }
     else if (strcmp((*arg)[1],cmnd[6])==0){ /* ls */
     printf(" List the control parameters or packets. \n\n");
     printf(" === List Control Parameters ===\n");
     printf(" ls n [range/point]\n");
     printf("     list nharm[]/ntr[]\n");
     printf(" trackorder example\n");
     printf("  ls n    (list all ntr[]\n");
     printf("  ls n 23   (list ntr[23]\n");
     printf("  ls n 23 45   (list ntr[23] to ntr[45]\n");
     printf(" frameorder example\n");
     printf("  ls n    (list all nnarm[]\n");
     printf("  ls n 23   (list nharm[23]\n");
     printf("  ls n 23 45   (list nharm[23] to nharm[45]\n\n");
     printf(" === List Packet Contents ===\n");
     printf(" ls [range/point] for track order\n");
     printf(" ls [range/point] [linerange]for frame order\n");
     printf("  range: a          ... all \n");
     printf("            23 34      #23 to #34\n");
     printf("  point: 23     ---> #23  \n");
     printf("  linerange:  line# to line#\n");
     printf(" `ls'  is the same as `ls a'\n");
     printf(" `ls .'  displays parameter range used for 'del' or 'pic'\n");
     printf("        CAUTION: not the resultant range run by 'del' or 'pic'\n");
     printf(" trackorder: examples:\n");
     printf("  ls a  (common for frameorder)\n");
     printf("  ls 23\n");
     printf("  ls 23 35 \n");
     printf(" frameorder example\n");
     printf("  ls 23       (frame# 23, all lines)\n");
     printf("  ls 23 35 47 (frame# 23 line#35 to 47)\n");
     printf("  ls a  12 34 (all frames line#12 to 34)\n");
     printf("  ls 12 24 10 24 (frames# 12 to 24 line# 10 to 24)\n");
     }
     else if (strcmp((*arg)[1],cmnd[7])==0){ /* ref */
      printf(" reference packet file in (with no argument)\n");
     }
     else if (strcmp((*arg)[1],cmnd[8])==0){ /* del */
      printf(" delete packets. \n");
     printf(" del [range/point] [thd/thu][pwth](undo possible by other command)\n");
     printf("  range: a          ... all \n");
     printf("            23 34      #23 to #34\n");
     printf("  point: 23  #23  \n");
     printf("  thd: below threshold are deleted.\n");
     printf("  thu: upper threshold are deleted.\n");
     printf("  pwth: linear level of threshold\n");
     printf("  usage examples:\n");
     printf(" del a\n");
     printf(" del 3\n");
     printf(" del 4 10\n");
     printf(" del frame# line#0 line#1 [frame order only]\n");
     printf(" del frame#0 frame#1 line#0 line#1 [frameorder only]\n");
     printf(" del thd (interactive question comes for pwth)\n");
     printf(" del thd 30.0\n");
     printf(" del thu (interactive question comes for pwth)\n");
     printf(" del thu 30.0\n");
     printf(" del 3 4 thd (interactive question comes for pwth)\n");
     printf(" del 3 7 thd 40.0\n");
     printf(" del 40 60 thu (interactive question comes for pwth)\n");
     printf(" del 50 57 thu 5.0\n");
     printf("Added command to 2nd version:\n");
     printf(" del wdw  (interactive question comes for rst)[track order only]\n");
     printf(" del wdw  359 [track order only]\n");
     printf(" del thwd (interactive question comes for pwthdb)\n");
     printf(" del thwd 30.0  (magnitude unit is in dB)\n");
     printf(" del thid (interactive question for pwth, meaning interval threshold)\n");
     printf("                                  [track order only];\n");
     printf(" del thid 5.0 [track order only]\n");
     printf(" del thiu (interactive. )[track order only]\n");
     printf(" del thiu 5.0[track order only]\n");
     printf(" del 100(Hz) 2000(Hz) thwd (interactive question comes for pwthdb)\n");
     printf(" del 100(Hz) 2000(Hz) thwd 5.0(dB)\n");
     }
     else if (strcmp((*arg)[1],cmnd[9])==0){ /* pic */
     printf(" pick is inverse meaning of del. \n");
     printf(" use pic after trying 'del a'\n");
      printf(" pic frames. \n");
     printf(" pic [range] (undo possible by other command)\n");
     printf("     range: a          ... all \n");
     printf("            23 34      #23 to #34\n");
     }
     else if (strcmp((*arg)[1],cmnd[10])==0){ /* e */
      printf(" e: edit packet contents. (interactive mode) \n");
      printf("    (no execution done without ru command)\n");
     }
     else if (strcmp((*arg)[1],cmnd[11])==0){ /* cv */
      printf(" cv: if   frame order,    then conversion to track order.\n");
      printf("     else (==track order),then conversion to frame order.\n");
     }
     else if (strcmp((*arg)[1],cmnd[12])==0){ /* zed */
      printf(" zed: to discard zero magnitude packets\n");
     }
     else if (strcmp((*arg)[1],cmnd[13])==0){ /* syn */
      printf(" syn: synthesize sound\n");
     }
     else if (strcmp((*arg)[1],cmnd[14])==0){ /* mat */
      printf(" mat: analysis data output for matlab (freq and magnitude)\n");
     }
     else {
      printf("No such command(%s) exists\n",(*arg)[1]);
     }
   }  /* end of hp with argument */

  }  /* end of hp command */

