#include <stdio.h>
#include <stdlib.h>

int Timbre_Paraset(double f0[], double Q[], int ds[], double *b2,
                   int id)
{
  int Nres,j;
  int Data0[9] = {-1,-1,-1,-1,-1,-1, 1, 1, 1}; // b2
  int Data[9][3][8] = {
// case 0: xylophone, especially s2
    260,1370,1930,2700,3100,3830,4500,6300,
    200,200,200,200,200,200,200,200,
    10,57,73,38,105,33,47,53,
// case 1: Steel Drum, especially s0
    136,166,465,571,700,1500,2200,3500,
    200,200,200,200,200,200,200,200,
    10,57,73,38,105,33,47,53,
// case 2: Wood ,especially s2. s1 is something
    136,166,465,571,700,900,1500,3700,
    10,10,10,10,10,10,50,50,
    10,57,73,38,105,45,23,51,
// case 3: Steel  (like piano) s1 is good, s2 is too dry but good.
    500,1000,1500,2000,2500,3000,3500,4000,
    200,200,200,200,200,200,200,200,
    10,57,73,38,105,45,51,63,
// case 4: Metal + Metal   with resonant B is -
    3000,4000,5000,7000,8000,9000,10000,12000,
    10,10,10,10,10,10,20,20,
    10,57,73,38,105,45,53,61,
// case 5: Metal + Metal   with resonant B is -, very attractive
// and real sound
    5000,7000,8000,9000,10000,12000,13000,14000,
    200,200,200,200,200,200,200,200,
    10,57,73,38,105,45,53,51,
// case 6: Wood and drums   with resonant B is +, real
    136,166,465,571,700,1500,2500,3500,
    70,30,20,20,10,10,10,20,
    70,57,13,38,25,45,53,61,
// case 7: piano sound with resonant B +
    440,880,1320,1760,2200,2640,3080,3520,
    400,400,400,400,400,400,400,400,
    70,57,13,38,25,45,53,61,
// case 8 good piano sound with resonant B +, harpsicode sound with 
//        resonant B -
    440,880,1320,1760,2200,2640,3080,3520,
    800,800,800,800,800,800,800,800,
    70,57,13,38,25,45,53,61
  };

  Nres = 8;
  for(j=0; j<Nres; j++) {
    f0[j] = (double)Data[id][0][j];
    Q[j]  = (double)Data[id][1][j];
    ds[j] =         Data[id][2][j];
  }
  *b2 = (double)Data0[id];
  return(Nres);
}
