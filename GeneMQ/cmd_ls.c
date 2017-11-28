#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
/* #include "sinupktNew.h"
#include "sinupktfrtr.h" */
#define MAXARG 10
#define MAXCOLUMN 100
/* -----------------------------------------------------------------
    Synthesis/Modification  operation using sinusoidal model's packet
    ls: list packets
          coded by Naotoshi Osaka May/21/2000
   ----------------------------------------------------------------- */
/* ===================================================================== */
int cmd_ls(struct sinuedparam *Qp,int narg,int ntrallcommon,int n_int,
   char *cmname[],char nametemp[][10],char ctemp[],FILE *input)
 {int i,ii,jj,isum;
 int ibias=12;

extern void get_trand(int j,int jold,int n_int,int npk,int iord[],
       struct trackpkt *tdum,FILE *input);
int rstL,rst,rstlL,rstl,renL,ren,renlL,renl;
double pwuthL,pwuth,pwdth,pwthdb,pwth;
int stfreqL,stfreq,enfreqL,enfreq;

   rstL   = Qp->rstL;
   rst    = Qp->rst;
   rstlL  = Qp->rstlL;
   rstl   = Qp->rstl;
   renL   = Qp->renL;
   ren    = Qp->ren ;
   renlL  = Qp->renlL;
   renl   = Qp->renl;
   pwuthL = Qp->pwuthL;
   pwuth  = Qp->pwuth;
   pwdth  = Qp->pwdth;
   pwthdb = Qp->pwthdb;
   stfreqL= Qp->stfreqL;
   stfreq = Qp->stfreq;
   enfreqL= Qp->enfreqL;
   enfreq = Qp->enfreq;
   pwth   = Qp->pwth;

 if(strcmp(arg[1],"n")==0 || strcmp(arg[1],"N")==0){ /* nharm/ntr monitor */
 if (narg==3){sscanf(arg[2],"%d",&rstL); renL=rstL;}
 else if(narg==4){sscanf(arg[2],"%d",&rstL); sscanf(arg[3],"%d",&renL); }
 if(trackorder==1){
   if(narg==2){rstL=0; renL=nframe-1;}
   isum=0;
   for(i=rstL;i<=renL;i++){isum += nharm[i];printf("nharm[%d]=%d \n",i,nharm[i]);}
   printf("-----------------\n Total: %d\n",isum);
 }
 else {  /* track order */
   if(narg==2){rstL=0; renL=ntrall-1;}
   isum=0;
   for(i=rstL;i<=renL;i++){isum += ntr[i]; printf(" ntr[%d]=%d \n",i,ntr[i]); }
   printf("-----------------\n Total: %d\n",isum);
 }
goto labelbase;

} /* end of cntrol par writing */

 else if(strcmp(arg[1],"a")==0 || narg==1){ 
    rstL=rstlL=pwuthL=stfreqL=  0.;
    renL= ndata+10;
    renlL=1000000;
    enfreqL=100000.;

    if(trackorder==0){renL=ntrallcommon-1;}
    else {
      renL= ndata+10;
      renlL=1000000;
    if(narg==4){ sscanf(arg[2],"%d",&rstlL);  sscanf(arg[3],"%d",&renlL); }
    }
 }
 else if (narg==2 && strcmp(arg[1],".")==0){/* monitor for pic/del */
  rstL=rst; rstlL= rstl; pwuthL=pwuth; stfreqL=stfreq;
  renL=ren; renlL=renl;   enfreqL=enfreq;


 if(ipick==1) printf("pick \n");
 else  printf("del \n");
 }
 else if (narg==2){sscanf(arg[1],"%d",&rstL); renL=rstL;}
 else if(narg==3){sscanf(arg[1],"%d",&rstL); sscanf(arg[2],"%d",&renL); }
 else if(narg==4){sscanf(arg[1],"%d",&rstL);
        sscanf(arg[2],"%d",&rstlL);  sscanf(arg[3],"%d",&renlL); renL=rstL;}
 else if(narg==5){sscanf(arg[1],"%d",&rstL);sscanf(arg[2],"%d",&renL);
        sscanf(arg[3],"%d",&rstlL);  sscanf(arg[4],"%d",&renlL); }
 else {printf(" ?? range unknown\n");  goto labelbase; }

if(trackorder==0){  /* ls command for track order */
 printf("                 Present packet monitoring \n");
 printf("------------------------+---------------+-------------------------\n");
 printf("     Original sound     |  Operation    |     Reference sound\n");
 printf("------------------------+---------------+-------------------------\n");
 printf(" TR  mean  mean fst lst |com0 com1 com2 | tr  mean  mean fst lst \n");
 printf("  #   fq    mag frm frm |Freq  Mag dely |  #   fq    mag frm frm\n");
 printf("------------------------+---------------+-------------------------\n");

 for(i=rstL;i<=renL;i++){
  strcpy(nametemp[0],cmname[icf[i]]);
  strcpy(nametemp[1],cmname[icm[i]+ibias]);
  strcpy(nametemp[2],cmname[icd[i]+2*ibias]);
  if(icf[i]==8){strcpy(nametemp[1],nametemp[0]);
                strcpy(nametemp[2],nametemp[0]);}
  if(icf[i]==2 || icf[i]==6 ||  icf[i]==7){sprintf(ctemp,"%3d",icf1[i]);
     strcat(nametemp[0],ctemp);}
  if(icf[i]==9){sprintf(ctemp,"%2d",icf1[i]);
     strcat(nametemp[0],ctemp);}
  if(icm[i]==2 ){sprintf(ctemp,"%3d",icm1[i]);
     strcat(nametemp[1],ctemp);}
  if(icm[i]==4 ){sprintf(ctemp,"%2d",icm1[i]);
     strcat(nametemp[1],ctemp);}
  if(icm[i]==5 ){sprintf(ctemp,"%2d",icm1[i]);
     strcat(nametemp[1],ctemp);}
  if(icm[i]==6 ){sprintf(ctemp,"%2d",icm1[i]);
     strcat(nametemp[1],ctemp);}
  if(icm[i]==7 ){sprintf(ctemp,"%2d",icm1[i]);
     strcat(nametemp[1],ctemp);}
  if(icd[i]==1){sprintf(ctemp,"%3d",icd1[i]);
     strcat(nametemp[2],ctemp);}
  if(icd[i]==2){sprintf(ctemp,"%1.1lf",cd1[i]);
     strcat(nametemp[2],ctemp);}


  if((ntrall==ntrall_ref) || (i<ntrall && i<ntrall_ref)){
     printf("%4d%6.0lf%6.0lf%3d%4d | %s %s %s|%4d%6.0lf%6.0lf%3d%4d\n",
       i,fmean0[i],magmean0[i],fstframe0[i],lstframe0[i],nametemp[0],
       nametemp[1],nametemp[2],
       i,fmean0[i],magmean0[i],fstframe0[i],lstframe0[i]);}
  else if(i>ntrall_ref){
  printf("%4d%6.0lf%6.0lf%3d%4d | %s %s %s|\n",
   i,fmean0[i],magmean0[i],fstframe0[i],lstframe0[i],nametemp[0],
   nametemp[1],nametemp[2]);}
  else{
  printf("                       |               |%4d%6.0lf%6.0lf%3d%4d\n",
   i,fmean0[i],magmean0[i],fstframe0[i],lstframe0[i]);}
} /* end of i */
 printf("------------------------+---------------+-------------------------\n");
} /* end of ls command for track order */

else { /* ls command for frame order */
  get_trand(0,0,n_int,npk,iord,&tdum,input); /* initiallize */
ii=rstL-1;
for(i=jj=0;i<ndata;i++){
 if(frtr[i].iframe>=rstL && frtr[i].iframe<=renL && 
    ((frtr[i].jtrack >=rstlL && frtr[i].jtrack <=renlL) ||frtr[i].jtrack<0)) {
 if(frtr[i].iframe!=ii){ ii=frtr[i].iframe;
 printf("----------------------------------------------------------------------------------\n");
 printf("         Packet monitor (Frame order)  iframe=%d nharm=%d\n",
        frtr[i].iframe,nharm[frtr[i].iframe]);
 printf("----+---------------+------------+----------+---+-------------------------------\n");
 printf(" TR |  nθ0    nθ1 | nω0  nω1 | M0   M1  |ir | iframe pk#\n");
 printf("  # |  (rad)   (rad)| (Hz)  (Hz) |(lin)(lin)|ut | \n");
 printf("----+---------------+------------+----------+---+-------------------------------\n");
 } /* end of header process */
  get_trand(i,jj,n_int,npk,iord,&tdum,input); 
  printf("%3d %8.4f%8.4f%6.0f%6.0f%6.0f%6.0f%2d%5d %5d\n",
   frtr[i].jtrack,(double)tdum.nTheta0/pow(2.,16.),
   (double)tdum.nTheta1/pow(2.,16.),
   (double)tdum.nOmega0/pow(2.,8.)/PI2,
   (double)tdum.nOmega1/pow(2.,8.)/PI2,
   (double)tdum.Mag0,(double)tdum.Mag1,
   irdie[i],
   frtr[i].iframe,i);
  jj=i;
 } /* end of frtr[i].iframe>=rstL && frtr[i].iframe<=renL */
 }
 printf("----+---------------+------------+----------+-----+-------------------------------\n");
} /* end of ls command for frame order */

labelbase:
   Qp->rstL    = rstL;
   Qp->rst     = rst;
   Qp->rstlL   = rstlL;
   Qp->rstl    = rstl;
   Qp->renL    = renL;
   Qp->ren     =  ren;
   Qp->renlL   =  renlL;
   Qp->renl    =  renl;
   Qp->pwuthL  = pwuthL;
   Qp->pwuth   = pwuth;
   Qp->pwdth   = pwdth;
   Qp->pwthdb  = pwthdb;
   Qp->stfreqL = stfreqL;
   Qp->stfreq  = stfreq;
   Qp->enfreqL = enfreqL;
   Qp->enfreq  = enfreq;
   Qp->pwth    = pwth;
  return(0);
}

