/* ------------------------------------------------------------
      Portnoff's Phase Vocoder header
                       coded by Naotoshi Osaka  12/28/1994
   ------------------------------------------------------------ */
/* buffer related in common */
#define NFFTP1 4097 // meaning nfft + 1. Other candiate might be 1025/2048
#define NFFT2 8192  
#define NSHIFT 500  /* = R */
#define LMAX  2 /*  6 */  /*  Low Pass Filter length relevant integer */
#define QMAX  2 /*  6 */  /*  Interpolate Filter length relevant integer */
#define Q2MAX   12  /*  2*Q */
#define LNFFTP1 /* 6145 */ 24577   /* =   L*NFFT + 1 */
#define L2NFFTP1 /* 12289 */  49153   /* = 2*L*NFFT + 1 */
#define Q2RP1     6001   /* = 2*Q*R+1 */

/*  sinusoidal analysis related */
#define NBUF  23000 /* 116000 */   /* $  more than (data byte)/2 */
/* see the definition of ndata in fsreadpf for the NBUF value */
/* general parameters */
#define PAI 3.14159265358979323846

/* system parameters */
#define SMSEC 10     /* frame shift in msec (int) */
#define SF    48.0  /*  sampling frequency in kHz */
