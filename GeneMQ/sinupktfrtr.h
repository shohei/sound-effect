/* -----------------------------------------------------------------
   packet definition
          coded by Naotoshi Osaka June/23/1994
            changed               Dec.1/1998
  Caution:
     The system alloacates 16 bytes to the struct below, and not 15 bytes
     Be careful. Therefore,
   Donot use
     fread(t,sizeof(short)*15,ndata,output).

   But use
     for(i=0;i<ndata;i++){fread(&t[i],sizeof(short),15,output);}

    |/|/|/|/|/|/|/|/|/|/|/|/|/|/|/| |/|/|/|/|/|/|/|/|/|/|/|/|/|/|/| |
    |<----          t[i]        --->|
  
  where  |/|  represents data, and | | represents vacant.
   ----------------------------------------------------------------- */
struct frtrpkt{
  int iframe;
  int jtrack;
};
