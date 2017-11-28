/* ==========================================================================
   initial data correction:
   In case the last data is not stopper, data are corrected.
   ========================================================================== */
int datacr_init(int iframeord)
{
 extern void *alocbuf(int unit,int nsize) ;
 int i,j,ii,jj,kk,ii0,ibs,nharmprev,nharmtemp;

  if(iframeord==1) {
 {free(frtr_ifnew); frtr_ifnew=NULL;}
   frtr_ifnew=alocbuf(sizeof(int),ndata+1); 
 {free(frtr_jtnew); frtr_jtnew=NULL;}
   frtr_jtnew=alocbuf(sizeof(int),ndata+1);
  {free(irdie_new); irdie_new=NULL;}
   irdie_new=alocbuf(sizeof(short),ndata+1);
  {free(frtr_ifsub); frtr_ifsub=NULL;}
   frtr_ifsub=alocbuf(sizeof(int),NENV);
  {free(frtr_jtsub); frtr_jtsub=NULL;}
   frtr_jtsub=alocbuf(sizeof(int),NENV);
  {free(irdie_sub); irdie_sub=NULL;}
   irdie_sub=alocbuf(sizeof(short),NENV);
  {free(iord1); iord1=NULL;}
   iord1=alocbuf(sizeof(int),NENV);

  for(i = ii = jj  = ibs = 0;i<nframe;i++){ /* jj starts count
                                             once new frame is encountered */

/* ----------------------------------------- 
       iord0 reset, iord1 reset, iord0 shift to iord, iord1 shift to iord
 */
   if(i==0){for(j=0;j<nharm[0];j++){
     frtr_ifnew[ii]=frtr[j].iframe;
     frtr_jtnew[ii]=frtr[j].jtrack;
     irdie_new[ii]= irdie[j];
     iord[ii++]= j;
   } 
   nharmprev=nharm[0];}
   else{
   ibs +=nharmprev;

   for(j=nharmtemp=jj=kk=ii0=0;j<nharm[i];j++){
    if(nharm[i]>NENV){
        printf("sinued Error. nharm[%d]=%d GT. NENV change pro. \n",i,nharm[i]);
        return(-4);
    }
       nharmprev=nharm[i];
       if(frtr[ibs+j].iframe== (i-1)){
          frtr_ifnew[ii]=frtr[ibs+j].iframe;
          frtr_jtnew[ii]=frtr[ibs+j].jtrack;
          irdie_new[ii]= irdie[ibs+j];
          iord[ii++]= ibs+j; nharm[i-1]++;kk++;
        }
       else if(frtr[ibs+j].iframe== i){
          frtr_ifsub[jj]=frtr[ibs+j].iframe;
          frtr_jtsub[jj]=frtr[ibs+j].jtrack;
          irdie_sub[jj]= irdie[ibs+j];
        iord1[jj++]= ibs+j; nharmtemp++;
       }
       else { ii0++;}
    } /* end of j loop */
   nharm[i]=nharmtemp;
   for(j=0;j<jj;j++){
          frtr_ifnew[ii]=frtr_ifsub[j];
          frtr_jtnew[ii]=frtr_jtsub[j];
          irdie_new[ii] = irdie_sub[j];
       iord[ii++]=iord1[j];
   }
  } /* end of else */

} /* end of i loop */

/* shift to orginal variables */
for(i=0;i<ndata;i++){
     frtr[i].iframe=frtr_ifnew[i];
     frtr[i].jtrack=frtr_jtnew[i];
     irdie[i]=irdie_new[i];
}

/* Monitor */
/* for(i=ii=0;i<nframe;i++){
    printf("III(frame#)=%d nharm[%d]=%d\n",i,i,nharm[i]);
    for(j=0;j<nharm[i];j++){
     printf("%d ",iord[ii++]);
    }
    printf("\n");
 } */

  free(frtr_ifnew); frtr_ifnew=NULL;
  free(frtr_jtnew); frtr_jtnew=NULL;
  free(irdie_new);  irdie_new=NULL;
  free(frtr_ifsub); frtr_ifsub=NULL;
  free(frtr_jtsub); frtr_jtsub=NULL;
  free(irdie_sub);  irdie_sub=NULL;
  free(iord1);      iord1=NULL;

 } /* end of iframord == 1 */
return(0);
}
