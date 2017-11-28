 void ordr(double data[],int n,int out[])
/* ---------------------------------------------
        Function to find the order of one dimensional data
        Example:  input: 3,16,39,5,-34
                  output:1,3,4,2,0
        In case of the same number in data, the first one
        is the smaller number
   Algorithm:
     Find the minimum number for the remaining data, and rank the number
        Coded by Naotoshi Osaka < aordr.f (1980/5/23)
         6/15/1994
 ------------------------------------------------- */

{ int i,jj,icont,in;
  double aminm;
    for(i=0;i<n;i++){out[i]= -1;}
    for(jj=0;jj<n;jj++){
      icont=0;
      for(i=0;i<n;i++){
       if(out[i]== -1){
        if(icont==0){aminm=data[i]; in=i;}
        icont++;
        if(data[i]<aminm){aminm=data[i];in=i;}
       }
      }
      out[in]=jj;
/*      printf("jj=%d out= %d %d %d %d %d\n",
          jj,out[0],out[1],out[2],out[3],out[4]); */
  }
}
/* ------------------------------------------------ */
/* main()
{
double data[5];
int j,out[5];
 data[0]= 3.0;
 data[1]= 16.0;
 data[2]= 34.0;
 data[3]= 5.0;
 data[4]= -34.0;
 ordr(data,5,out);
 printf("Result:  out= %5d %5d %5d %5d %5d\n",
          out[0],out[1],out[2],out[3],out[4]);
}

*/
