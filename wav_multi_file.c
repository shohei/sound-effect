#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "wavlib.h"
#define BUFFER_COUNT 2 // �}���`�o�b�t�@��

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT Message, 
                             WPARAM wParam, LPARAM lParam);

static LPSTR lpClassName = "My Class";
static LPSTR lpClassTitle = "First WindowsApplication";
static LPSTR lpMenuName = "My Menu";

wav_head bufhead;
static char InFileN[] = {"C:\\src\\fot.bak.441.wav"};
int bufferSize;    // wave�o�b�t�@�T�C�Y(byte)
int bufferLength;  // wave�o�b�t�@��(�f�[�^��/s)
int bufferSelect;  // �o�b�t�@�I��ԍ�
static short *(wWave[BUFFER_COUNT]); // wave�o�b�t�@(�}���`)

static HWAVEOUT hWave;
static WAVEHDR whdr[BUFFER_COUNT];
int waveDataSize;  // �Đ��\���WAVE�f�[�^�T�C�Y
int readDataSize;  // �Đ��ς�WAVE�f�[�^�T�C�Y
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
    MessageBox(0, "�G���[�F�E�B���h�E���o�^","MyApp", MB_OK);
    return FALSE;
  }
  if(!(hWndMain = 
    CreateWindow(lpClassName,lpClassTitle,WS_OVERLAPPEDWINDOW,
                 CW_USEDEFAULT,CW_USEDEFAULT,100,100,
                 HWND_DESKTOP,NULL,hInstance,NULL)))
    return FALSE;

  ShowWindow(hWndMain, nCmdShow);
  UpdateWindow(hWndMain);
  if(initWave(hWndMain) == FALSE)return FALSE; // wave������
  while(GetMessage(&msg, NULL, 0, 0)) {
    DispatchMessage(&msg);
  }
  closeWave(); // �㏈��
  return msg.wParam;
}
/* ------------------
    Window Procedure
   ------------------*/
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT Message, 
                             WPARAM wParam, LPARAM lParam)
{
  switch (Message) {

  case MM_WOM_DONE:// 1�̃o�b�t�@�̃f�[�^�Đ��I�����ɌĂяo�����
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
    bufhead.fs * bufhead.Nch;      // fs * channel�f�[�^��/s
  bufferSize =
    bufferLength * sizeof(short);  // �o�b�t�@�T�C�Y�kbyte�l
  for(i = 0; i < BUFFER_COUNT; i++) {
    wWave[i] = 
      (short *)malloc(bufferSize); // wWave�ɗ̈抄�蓖��
  }

  // �o�̓f�o�C�X�̐ݒ�
  wfe.wBitsPerSample = bits;  // bits is in wavlib.h
  wfe.nChannels = bufhead.Nch;
  wfe.wFormatTag = WAVE_FORMAT_PCM;
  wfe.nSamplesPerSec = 
    (int)bufhead.fs;          // fs�@�T���v�����O���g���kHz�l
  wfe.nBlockAlign = wfe.nChannels * wfe.wBitsPerSample / 8;
  wfe.nAvgBytesPerSec = wfe.nSamplesPerSec * wfe.nBlockAlign;
  wfe.cbSize = waveDataSize = bufhead.Nbyte;

  waveOutOpen(&hWave , WAVE_MAPPER, &wfe,   // �o�̓f�o�C�X���J��
              (DWORD)hWnd, 0, CALLBACK_WINDOW);

  for(i = 0; i < BUFFER_COUNT; i++) {
    // �Đ��o�b�t�@�Ƀf�[�^�𑗂邽�߂̐ݒ�
    whdr[i].lpData = (LPSTR)wWave[i];
    whdr[i].dwBufferLength = 0;
    whdr[i].dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
    whdr[i].dwLoops = 1;

    waveOutPrepareHeader(hWave, &(whdr[i]), sizeof(WAVEHDR));
  }

  readDataSize = 0;
  // �_�u���o�b�t�@�����O�����邽�߂ɋ�f�[�^����������
  waveOutWrite(hWave , &(whdr[0]) , sizeof(WAVEHDR));
  waveOutWrite(hWave , &(whdr[1]) , sizeof(WAVEHDR));

  // �o�b�t�@�ԍ��̐ݒ�
  bufferSelect = 2%BUFFER_COUNT;
  return TRUE;
}
/* ------------
   Wave�̌㏈��
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
�o�b�t�@�̏I�����ɌĂ΂��
  ---------------------- */
LRESULT wmWomDone(void)
{
  int readsize,i,idlast;
  // �Ō�܂ōĐ������ꍇ�͍ŏ��Ƀ������̃|�C���^��߂�
  if(waveDataSize - readDataSize == 0) {
    readDataSize = 0;
    // MM_WOM_DONE�����������߂�waveOutWrite���Ă�
    whdr[bufferSelect].dwBufferLength = 0;
    waveOutWrite((HWAVEOUT)hWave,&(whdr[bufferSelect]),
                 sizeof(WAVEHDR));

    bufferSelect = 
      (bufferSelect+1)%BUFFER_COUNT; // �o�b�t�@�̃`���l���I��
    return 0;
  }

  readsize = bufferSize;  // �ǂݍ��݃T�C�Y
  if(readsize > waveDataSize - readDataSize) {
    readsize = waveDataSize - readDataSize;
    idlast = 1;
  } else {
    idlast = 0;
  }
  /* -------------------------------------------------------
   �ǂݍ��� bufhead.data �ɏ������ނ��CwWave[bufferSelect]��
   ���ڃf�[�^����������
    ------------------------------------------------------- */
  for (i=0; i < readsize/2; i++) {
    wWave[bufferSelect][i] = bufhead.data[readDataSize/2+i];
  }
  if(idlast==1) readDataSize = waveDataSize;
  else          readDataSize += readsize;
  // �Đ��o�b�t�@�ɏ�������
  whdr[bufferSelect].dwBufferLength = readsize;
  waveOutWrite((HWAVEOUT)hWave , &(whdr[bufferSelect]) ,
               sizeof(WAVEHDR));

  bufferSelect = 
    (bufferSelect+1)%BUFFER_COUNT;   // �o�b�t�@�ԍ��̍X�V
  return 0;
}
