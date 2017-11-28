static double lastUsedf,lastCons1,lastCons2;
int sine_func(short *data, int strt_count, int n_data, int fs,
              double amp, double f, int id)
{
  int i,j;
  double cons,a,a1,p,p1,w,w1,ftemp,temp,temp1;
  double lastf,lastc1,lastc2;

  lastf  = lastUsedf;
  lastc1 = lastCons1;
  lastc2 = lastCons2;
  cons   = 2.*pi/(double)nbuf;

  if(id==0) {
    for (i=0; i<nbuf; i++) {
      sintbl[i] = sin(cons*(double)i);
    }
    return strt_count;
  } else {
    j = strt_count;
    for(i=0; i<n_data; i++) {
      w = cons2*fmod((double)j,cons1);       // ‹˜”g (phasor)
      p = (double)nbuf*w;
      temp = amp*((1.- w) * sintbl[(s_k=(int)p)] 
                  + w*sintbl[s_k==(nbuf-1)? 0 : s_k+1]);
      if(idf==1) {
        data[i] = (short)temp;

      } else { // idf==0
        w1 = lastc2*fmod((double)j,lastc1);  // ‹˜”g (phasor)
        p1 = (double)nbuf*w1;
        temp1 = amp*((1.- w1) * sintbl[(s_k1=(int)p1)] 
                     + w1*sintbl[s_k1==(nbuf-1)? 0 : (s_k1+1)]);
        data[i] = (short)((1.0 - djf) * temp1 + djf * temp);
        if(djf>=1.) idf=1;
        djf += 0.0008;
      }   // end of else (idf==0)
      j++;
    } // end of for i
    lastUsedf = f;
    lastCons1 = cons1;
    lastCons2 = cons2;
    return j;
  }
}
