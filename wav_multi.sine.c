#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int fs=44100;     // �T���v�����O���g���kHz�l
double pi=3.14159265358979323846;

double f=357.43 ; // �M���̎��g���kHz�l
int rdpara, n_data;
double amp=10000.;
int sine_func(short *data, int strt_count, int n_data, int fs,
              double amp, double f);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam)
{
  WAVEFORMATEX wfe;
  static HWAVEOUT hWaveOut;
  static WAVEHDR whdr[2];
  static short  *lpWave[2];
  int i,len,k,idur=1;
  static int k0=1;
  double d;
  FILE *fp;
  short ibuf[96000];

  static int buf_num;
  HDC hdc;
  PAINTSTRUCT ps;
  char str[32],str1[32];
  MMTIME mmt;
  static int ms;

  switch(uMsg) {
  case WM_CREATE:
    wfe.wFormatTag = WAVE_FORMAT_PCM;
    wfe.nChannels = 1;          // ���m����
    wfe.wBitsPerSample = 16;    // �ʎq��bit��
    wfe.nBlockAlign = wfe.nChannels * wfe.wBitsPerSample/8;
    wfe.nSamplesPerSec = fs;    // �W�{�����g��
    wfe.nAvgBytesPerSec = wfe.nSamplesPerSec * wfe.nBlockAlign;

    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfe, (DWORD)hWnd, 0,
                CALLBACK_WINDOW);

    len = fs/f;	// �g��
    rdpara = 0;
    n_data = fs*idur;
    for(k=0; k<2; k++) {
      lpWave[k] =
        (short *)calloc(wfe.nAvgBytesPerSec,idur);   // idur �b��
      rdpara = 
        sine_func(lpWave[k],rdpara,n_data,fs,amp,f); // �g�`�f�[�^�쐬
      whdr[k].lpData = (LPSTR)lpWave[k];
      whdr[k].dwBufferLength = wfe.nAvgBytesPerSec * idur;
      whdr[k].dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
      whdr[k].dwLoops = 1;

      waveOutPrepareHeader(hWaveOut,&whdr[k],sizeof(WAVEHDR));
    }

    fclose(fp);
    SetTimer(hWnd,1,50,NULL);
    return 0;
  case WM_LBUTTONDOWN:
    for(k=0; k<2; k++) {
      waveOutWrite(hWaveOut,&whdr[k],sizeof(WAVEHDR));
      whdr[k].dwUser = 0;
      buf_num = k;
    }
    return 0;
  case WM_RBUTTONDOWN:
    waveOutReset(hWaveOut);
    for(k=0; k<2; k++)whdr[k].dwUser=1; // waveOutReset�֐����Ăяo����
    return 0;
  case WM_TIMER:
    mmt.wType = TIME_SAMPLES;
    waveOutGetPosition(hWaveOut,&mmt,sizeof(MMTIME));
    switch(mmt.wType) {
    case TIME_MS:
      ms = mmt.u.ms;
      break;
    case TIME_SAMPLES:
      ms = mmt.u.sample;
//      ms = 1000 * mmt.u.sample / wfe.nSamplesPerSec;
      break;
    case TIME_BYTES:
      ms = 1000 * mmt.u.cb / wfe.nAvgBytesPerSec;
      break;
    default:	// ��Ή�
      ms = -1;
      break;
    }
    InvalidateRect(hWnd,NULL,TRUE);
    return 0;
  case MM_WOM_DONE:
    if(((LPWAVEHDR)lParam)->dwUser) { // waveOutReset�֐��Œ�~����
      buf_num = 0;     // �����l�ɖ߂�
    } else {           // �Ō�܂ōĐ�����
      waveOutWrite((HWAVEOUT)wParam, (LPWAVEHDR)lParam,
                   sizeof(WAVEHDR));
      for(k=0; k<2; k++) {
        if((LPWAVEHDR)lParam==&whdr[k]) {
          buf_num = k; // �Đ����I�������o�b�t�@
          k0++;
          rdpara = sine_func(lpWave[buf_num],rdpara,n_data,fs,amp,f);
          break;
        }
      }
      buf_num = (buf_num+1)%2;  // �Đ����̃o�b�t�@

    }
    InvalidateRect(hWnd,NULL,TRUE);
    return 0;
  case WM_PAINT:
    hdc = BeginPaint(hWnd,&ps);
    wsprintf(str,"�Đ��T���v���ʒu�F�b:= %07d:%05d:%05d\n",ms,
             (ms/fs),ms%fs);
    wsprintf(str1,"�Đ����̃o�b�t�@ = %d\n",buf_num);
    TextOut(hdc,0,0,str,lstrlen(str));

    EndPaint(hWnd,&ps);
    return 0;
  case WM_DESTROY:
    waveOutReset(hWaveOut);
    for(k=0; k<2; k++)
      waveOutUnprepareHeader(hWaveOut,&whdr[k],sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
    for(k=0; k<2; k++)free(lpWave[k]);
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   PSTR lpCmdLine,int nCmdShow)
{
  HWND hWnd;
  WNDCLASS wc;
  MSG msg;
  BOOL bRet;

  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL,IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = __FILE__;
  if(!RegisterClass(&wc)) return 0;

  hWnd = CreateWindow(__FILE__,"WAVE�̃}���`�o�b�t�@�����O",
                      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                      300,300,
                      800,500,
                      NULL,NULL,hInstance,NULL);
  if(hWnd==NULL) return 0;

  while((bRet=GetMessage(&msg,NULL,0,0))!=0) {
    if(bRet==-1) break;
    DispatchMessage(&msg);
  }
  return (int)msg.wParam;
}
int sine_func(short *data,int strt_count,int n_data,
              int fs,double amp, double f)
{
  int i,j;
  double cons;
  cons = 2.*pi*f/(double)fs;
  j = strt_count;
  for(i=0; i<n_data; i++)
    data[i] = (short)(amp*sin(cons*(double)j++ ));
  return j;
}
