#include <stdlib.h>
#include <math.h>

int inv3(double a[4][4], double ainv[4][4])
{
  double detA;
  double A[9],xb[8];
  detA = a[1][1]*a[2][2]*a[3][3]+a[2][1]*a[3][2]*a[1][3]
         +a[3][1]*a[1][2]*a[2][3]-a[1][1]*a[3][2]*a[2][3]
         -a[3][1]*a[2][2]*a[1][3]-a[2][1]*a[1][2]*a[3][3];
  ainv[1][1] = (a[2][2]*a[3][3]-a[2][3]*a[3][2])/detA;
  ainv[1][2] = (a[1][3]*a[3][2]-a[1][2]*a[3][3])/detA;
  ainv[1][3] = (a[1][2]*a[2][3]-a[1][3]*a[2][2])/detA;
  ainv[2][1] = (a[2][3]*a[3][1]-a[2][1]*a[3][3])/detA;
  ainv[2][2] = (a[1][1]*a[3][3]-a[1][3]*a[3][1])/detA;
  ainv[2][3] = (a[1][3]*a[2][1]-a[1][1]*a[2][3])/detA;
  ainv[3][1] = (a[2][1]*a[3][2]-a[2][2]*a[3][1])/detA;
  ainv[3][2] = (a[1][2]*a[3][1]-a[1][1]*a[3][2])/detA;
  ainv[3][3] = (a[1][1]*a[2][2]-a[1][2]*a[2][1])/detA;
  return(0);
}
