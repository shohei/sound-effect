#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "wavlib.h"
#define BUFFER_COUNT 2 // マルチバッファ数

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT Message, 
                             WPARAM wParam, LPARAM lParam);

static LPSTR lpClassName = "My Class";
static LPSTR lpClassTitle = "First WindowsApplication";
static LPSTR lpMenuName = "My Menu";

wav_head bufhead;
static char InFileN[] = {"C:\\src\\fot.bak.441.wav"};
int bufferSize;    // waveバッファサイズ(byte)
int bufferLength;  // waveバッファ長(データ数/s)
int bufferSelect;  // バッファ選択番号
static short *(wWave[BUFFER_COUNT]); // waveバッファ(マルチ)

static HWAVEOUT hWave;
static WAVEHDR whdr[BUFFER_COUNT];
int waveDataSize;  // 再生予定のWAVEデータサイズ
int readDataSize;  // 再生済のWAVEデータサイズ
int initWave(HWND hWnd);
void closeWave(void);
LRESULT wmWomDone(void);
/* -----------
   Main Program
    ------------ */
INT WINAPI WinMain(HINSTANCE hInstance, 
                   HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, INT nCmdShow)
{
  HWND hWndMain;
  MSG msg;
  WNDCLASSEX   wndclass;

  wndclass.cbSize = sizeof(WNDCLASSEX);
  wndclass.hInstance = hInstance;
  wndclass.lpszClassName = lpClassName;
  wndclass.lpfnWndProc = MainWndProc;
  wndclass.style = 0;
  wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclass.lpszMenuName = NULL;
  wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
  wndclass.hbrBackground=(HBRUSH) GetStockObject(WHITE_BRUSH);
  if(!RegisterClassEx(&wndclass)) {
    MessageBox(0, "エラー：ウィンドウ未登録","MyApp", MB_OK);
    return FALSE;
  }
  if(!(hWndMain = 
    CreateWindow(lpClassName,lpClassTitle,WS_OVERLAPPEDWINDOW,
                 CW_USEDEFAULT,CW_USEDEFAULT,100,100,
                 HWND_DESKTOP,NULL,hInstance,NULL)))
    return FALSE;

  ShowWindow(hWndMain, nCmdShow);
  UpdateWindow(hWndMain);
  if(initWave(hWndMain) == FALSE)return FALSE; // wave初期化
  while(GetMessage(&msg, NULL, 0, 0)) {
    DispatchMessage(&msg);
  }
  closeWave(); // 後処理
  return msg.wParam;
}
/* ------------------
    Window Procedure
   ------------------*/
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT Message, 
                             WPARAM wParam, LPARAM lParam)
{
  switch (Message) {

  case MM_WOM_DONE:// 1つのバッファのデータ再生終了時に呼び出される
    return wmWomDone();
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hwnd, Message, wParam, lParam);
  }
  return FALSE;
}
/* ----------------------------------------
 Initialization of Wave relevant procedure
 ------------------------------------------ */
int initWave(HWND hWnd)
{
  int i;
  WAVEFORMATEX wfe;

  wavread(&bufhead,InFileN);
  bufferLength = 
    bufhead.fs * bufhead.Nch;      // fs * channelデータ数/s
  bufferSize =
    bufferLength * sizeof(short);  // バッファサイズ〔byte〕
  for(i = 0; i < BUFFER_COUNT; i++) {
    wWave[i] = 
      (short *)malloc(bufferSize); // wWaveに領域割り当て
  }

  // 出力デバイスの設定
  wfe.wBitsPerSample = bits;  // bits is in wavlib.h
  wfe.nChannels = bufhead.Nch;
  wfe.wFormatTag = WAVE_FORMAT_PCM;
  wfe.nSamplesPerSec = 
    (int)bufhead.fs;          // fs　サンプリング周波数〔Hz〕
  wfe.nBlockAlign = wfe.nChannels * wfe.wBitsPerSample / 8;
  wfe.nAvgBytesPerSec = wfe.nSamplesPerSec * wfe.nBlockAlign;
  wfe.cbSize = waveDataSize = bufhead.Nbyte;

  waveOutOpen(&hWave , WAVE_MAPPER, &wfe,   // 出力デバイスを開く
              (DWORD)hWnd, 0, CALLBACK_WINDOW);

  for(i = 0; i < BUFFER_COUNT; i++) {
    // 再生バッファにデータを送るための設定
    whdr[i].lpData = (LPSTR)wWave[i];
    whdr[i].dwBufferLength = 0;
    whdr[i].dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
    whdr[i].dwLoops = 1;

    waveOutPrepareHeader(hWave, &(whdr[i]), sizeof(WAVEHDR));
  }

  readDataSize = 0;
  // ダブルバッファリングをするために空データを書き込み
  waveOutWrite(hWave , &(whdr[0]) , sizeof(WAVEHDR));
  waveOutWrite(hWave , &(whdr[1]) , sizeof(WAVEHDR));

  // バッファ番号の設定
  bufferSelect = 2%BUFFER_COUNT;
  return TRUE;
}
/* ------------
   Waveの後処理
   ------------*/
void closeWave(void)
{
  int i;
  waveOutReset((HWAVEOUT)hWave);
  for(i=0; i < BUFFER_COUNT; i++) {
    waveOutUnprepareHeader((HWAVEOUT)hWave, &(whdr[i]),
                           sizeof(WAVEHDR));
    free(wWave[i]);
  }
  free(bufhead.data);
}
/* -----------------------
バッファの終了時に呼ばれる
  ---------------------- */
LRESULT wmWomDone(void)
{
  int readsize,i,idlast;
  // 最後まで再生した場合は最初にメモリのポインタを戻す
  if(waveDataSize - readDataSize == 0) {
    readDataSize = 0;
    // MM_WOM_DONEをおこすためにwaveOutWriteを呼ぶ
    whdr[bufferSelect].dwBufferLength = 0;
    waveOutWrite((HWAVEOUT)hWave,&(whdr[bufferSelect]),
                 sizeof(WAVEHDR));

    bufferSelect = 
      (bufferSelect+1)%BUFFER_COUNT; // バッファのチャネル選択
    return 0;
  }

  readsize = bufferSize;  // 読み込みサイズ
  if(readsize > waveDataSize - readDataSize) {
    readsize = waveDataSize - readDataSize;
    idlast = 1;
  } else {
    idlast = 0;
  }
  /* -------------------------------------------------------
   読み込み bufhead.data に書き込むか，wWave[bufferSelect]に
   直接データを書き込む
    ------------------------------------------------------- */
  for (i=0; i < readsize/2; i++) {
    wWave[bufferSelect][i] = bufhead.data[readDataSize/2+i];
  }
  if(idlast==1) readDataSize = waveDataSize;
  else          readDataSize += readsize;
  // 再生バッファに書き込み
  whdr[bufferSelect].dwBufferLength = readsize;
  waveOutWrite((HWAVEOUT)hWave , &(whdr[bufferSelect]) ,
               sizeof(WAVEHDR));

  bufferSelect = 
    (bufferSelect+1)%BUFFER_COUNT;   // バッファ番号の更新
  return 0;
}
