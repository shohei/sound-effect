/* -----------------------------------------------------------------
   readpkt.c
     read packet from "*.pkt" file
     file is already open  (input),and header is already read.
      coded yb Naotoshi Osaka
         April 30, 2000
   ----------------------------------------------------------------- */
int  readpkt(FILE *input,int *iframeord,int n_int,int idw[])
{ extern void *alocbuf(int unit,int nsize);
  extern void get_t(int j,int n_int,int npk,int iord[],struct trackpkt *tdum,
       FILE *input);
         void pktmonitor(int i,int idw,struct trackpkt tdum);
  int i,ie;

/* ********* M.A. ********* */
 iord=alocbuf(sizeof(int),ndata+1);
 irdie=alocbuf(sizeof(short),ndata+1);
 frtr=alocbuf(sizeof(struct frtrpkt),ndata+1);
/* ******** E. M.A. ******** */
 /* Packet read and monitor */
 for(i=0;;i++){
    if(i>ndata){ /* packet size exceeds than ndata */
      return(-10);
    }
    if(ie=(int)fread(&tdum,sizeof(short),npk,input) != npk) {printf("i=%d\n",i);break;}
    frtr[i].iframe=tdum.iframe;
    frtr[i].jtrack=tdum.jtrack;
    irdie[i]=tdum.iroute;
    iord[i] = i; 
/* if(frtr[i].jtrack== -1 || tdum.iroute == (short)1 || tdum.iroute == (short)2 ||
          tdum.iroute == (short)5) */ {  
    if(idw[3]==1){  printf("i=%d iframe=%d jtrack=%d iroute=%d\n",
      i,frtr[i].iframe,frtr[i].jtrack,tdum.iroute);}
}
   if(idw[1]>0){ pktmonitor(i,idw[1],tdum);}
  else { pktmonitor(i,idw[2],tdum);}  
} /* out of endless loop */


printf("i=%d\n",i);
 frtr[i].iframe=tdum.iframe;
printf("actual number of packets=%d frame_stopper=%d\n",i,frtr[i].iframe);
if(i != ndata){
      printf("CAUTION! This sinupkt is from wrong sinu version.(ndata=%d\n",
      ndata);}

  if(frtr[i].iframe == -100)*iframeord=0; else *iframeord=1; 
/* iframeord:1 raw; very first time after sinu, 0: order corrected */
  ndata=i;
  printf("ndata=%d iframeord=%d\n",ndata,*iframeord);
  /*  if(ndata>=ndatamax){ return(-3); } */
/*   fclose(input);   */

printf("n_int=%d\n",n_int);
/* for(i=0;i<20;i++){
   get_t(i,n_int,npk,iord,&tdum,input); 
  pktmonitor(i,1,tdum);
} */
return(0);
}
