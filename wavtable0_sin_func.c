static double lastUsedf;
int sine_func(short *data, int strt_count, int n_data, int fs,
              double amp, double f, int id)
{
  int i,j;
  double cons,a,a1,w,w1,ftemp,temp,temp1;
  double lastf;

  lastf = lastUsedf;
  cons  = 2.*pi/(double)nbuf;

  if(id==0) {
    for (i=0; i<nbuf; i++) {
      sintbl[i] = sin(cons*(double)i);
    }
    return strt_count;
  } else {
    j = strt_count;
    for(i=0; i<n_data; i++) {
      a = fmod(f*(double)nbuf*(double)j/(double)fs,(double)nbuf);
      w = a/(double)nbuf;
      temp = amp*((1.- w) * sintbl[(s_k=(int)a)] 
                  + w*sintbl[s_k==(nbuf-1)? 0 : s_k+1]);
      if(idf==1) {
        data[i] = (short)temp;

      } else { // idf==0
        a1 = fmod(lastf*(double)nbuf*(double)j/
                  (double)fs,(double)nbuf);
        w1 = a1/(double)nbuf;
        temp1 = amp*((1.- w1) * sintbl[(s_k1=(int)a1)] 
                     + w1*sintbl[s_k1==(nbuf-1)? 0 : (s_k1+1)]);
        data[i] = (short)((1.0 - djf) * temp1 + djf * temp);
        if(djf>=1.) idf=1;
        djf += 0.0007;
      }   // end of else (idf==0)
      lastUsedf = f;
      j++;
    } // end of for i
    return j;
  }
}
