#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define N_BUFFER 2 //�}���`�o�b�t�@���i�ʏ�Q�j

int    fs=44100;    //�T���v�����O���g���@(Hz);
double f =357.43 ;  // �M���̎��g��(Hz)
double amp=10000.;
double pi=3.14159265358979323846;

int rdpara, n_data;

int sine_func(short *data,int strt_count,int n_data, int fs,double amp, double f);

static LPSTR lpClassName = "My Class";
static LPSTR lpClassTitle = "Music Keyboard for Harmonics";

// GUI �֘A
static int nRect=12, width=50;
static int iRect=0,lRect=0; // ���݂�Rect#�C�@���O��Rect# 0:�͈͊O
static int Isplaying=0;  //0: ���t��~��(=closed), 1: ������ 2: ���t��
static int IDMouseUse;

/* ------------------------------------------------------------------------ */
LRESULT CALLBACK WindowProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
    WAVEFORMATEX wfe;
    static HWAVEOUT hWaveOut;
    static WAVEHDR whdr[N_BUFFER];
    static RECT rct;
    static int nLeftRect[12],nRightRect[12],nBottomRect[12],nTopRect[12];
    static short  *lpWave[N_BUFFER];
    int i,k,idur=1;
    double d;
    static int buf_num;
    HDC hdc;
    PAINTSTRUCT ps;
    char str[32],str1[32];
    MMTIME mmt;
    static BOOL blMouse = FALSE;


    switch(Msg) {
    case WM_CREATE:
        wfe.wFormatTag=WAVE_FORMAT_PCM;
        wfe.nChannels=1;	//���m����
        wfe.wBitsPerSample=16;	//�ʎq���r�b�g��
        wfe.nBlockAlign=wfe.nChannels * wfe.wBitsPerSample/8;
        wfe.nSamplesPerSec=fs;	//�W�{�����g��
        wfe.nAvgBytesPerSec=wfe.nSamplesPerSec * wfe.nBlockAlign;

        waveOutOpen(&hWaveOut,WAVE_MAPPER,&wfe,(DWORD)hwnd,0,CALLBACK_WINDOW);

        rdpara=0;
        n_data=fs*idur;
        for(k=0; k<N_BUFFER; k++) {
            lpWave[k]=(short *)calloc(wfe.nAvgBytesPerSec,idur);	//idur �b��
            rdpara=sine_func(lpWave[k],rdpara,n_data, fs,amp, f);	//�g�`�f�[�^�쐬
            whdr[k].lpData=(LPSTR)lpWave[k];
            whdr[k].dwBufferLength=wfe.nAvgBytesPerSec * idur;
            whdr[k].dwFlags=WHDR_BEGINLOOP | WHDR_ENDLOOP;
            whdr[k].dwLoops=1;
            waveOutPrepareHeader(hWaveOut,&whdr[k],sizeof(WAVEHDR));
        }
        return 0;
    case WM_LBUTTONDOWN:
        lRect=iRect;
        blMouse = TRUE;
        rct.left = LOWORD(lParam);
        rct.top = HIWORD(lParam);
        for(i=iRect=0; i<nRect; i++) {
            if ((rct.left >nLeftRect[i] && rct.left<nRightRect[i]) &&
                    (rct.top>nTopRect[i] && rct.top<nBottomRect[i])) {
                iRect=i+1; // iRect �ԍ��i���Ղ̍�����̔ԍ�)��1�n�܂�
                break;  // iRect�m��
            }
        }

        if (iRect!=0 && (lRect==iRect ||  lRect==0 || lRect!=0) ) { //���Տ�
            //�}���`�o�b�t�@�����O�����邽�߂ɋ�f�[�^����������
            if(lRect!=0) { // ���݂͌��Տォ���O�����Տ�
                if (Isplaying>0) {
                    Isplaying=0;
                    waveOutReset((HWAVEOUT)hWaveOut); // ���݂̔����𒆎~
                    //  �������@whdr[i].dwUser=1;�͍s��Ȃ�
                    IDMouseUse=0;
                }
            } else { // ���݂͌��Տ�A���O�͌��ՊO  IDMouseUse==0

                for(k=0; k<N_BUFFER; k++) {
                    waveOutWrite(hWaveOut,&whdr[k],sizeof(WAVEHDR));
                    whdr[k].dwUser=0;
                    buf_num=k;
                }
            }
            Isplaying=1; // ������
            IDMouseUse=1; // DA�p�Ƀ}�E�X�g�p
        } else if (Isplaying>0) { // ���ՊO�@DA��~
            Isplaying=0;

            waveOutReset(hWaveOut);
            for(k=0; k<N_BUFFER; k++)whdr[k].dwUser=1; //waveOutReset�֐��̌Ăяo��
            IDMouseUse=0;

        }
        return 0;
    case WM_RBUTTONDOWN:
        waveOutReset(hWaveOut);
        for(k=0; k<N_BUFFER; k++)whdr[k].dwUser=1; //waveOutReset�֐����Ăяo��
        return 0;

    case MM_WOM_DONE:
        if(((LPWAVEHDR)lParam)->dwUser) {	//waveOutReset�֐��ł̒�~���󂯂�
            buf_num=0;		//�����l�ɖ߂�
        } else {		//�Ō�܂ōĐ�����
            waveOutWrite((HWAVEOUT)wParam,(LPWAVEHDR)lParam,sizeof(WAVEHDR));
            for(k=0; k<N_BUFFER; k++) {
                if((LPWAVEHDR)lParam==&whdr[k]) {
                    buf_num=k;		//�Đ����I�������o�b�t�@
                    rdpara=sine_func(lpWave[buf_num],rdpara,n_data, fs,amp, f*iRect);
                    break;
                }
            }
            buf_num=(buf_num+1)%N_BUFFER;  //�Đ����̃o�b�t�@

        }
        InvalidateRect(hwnd,NULL,TRUE);
        return  0;
    case WM_PAINT:
        hdc=BeginPaint(hwnd,&ps);
        for(i=0; i<nRect; i++) {
            nLeftRect[i]=(i==0? 100:  nRightRect[i-1]);
            nTopRect[i]=120;
            nRightRect[i]=nLeftRect[i]+width;
            nBottomRect[i]=250;
            SelectObject(hdc , CreateSolidBrush(0xBB));
            Rectangle(hdc , nLeftRect[i],nTopRect[i],nRightRect[i],nBottomRect[i]);
        }

        EndPaint(hwnd,&ps);
        return 0;
    case WM_DESTROY:
        waveOutReset(hWaveOut);
        for(k=0; k<N_BUFFER; k++)waveOutUnprepareHeader(hWaveOut,&whdr[k],sizeof(WAVEHDR));
        waveOutClose(hWaveOut);
        for(k=0; k<N_BUFFER; k++)free(lpWave[k]);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd,Msg,wParam,lParam);
    }
}
/* ------------------------------------------------------------------ */
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   PSTR lpCmdLine,int nCmdShow)
{
    HWND hwnd;
    WNDCLASSEX wc;
    MSG msg;
    BOOL bRet;

    wc.style		 = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc	 = WindowProc;
    wc.cbClsExtra	 = 0;
    wc.cbWndExtra	 = 0;
    wc.hInstance	 = hInstance;
    wc.hIcon		 = LoadIcon(NULL,IDI_APPLICATION);
    wc.hCursor		 = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName	 = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = lpClassName;
    wc.hIconSm =LoadIcon(NULL, IDI_WINLOGO);
    if(!RegisterClassEx(&wc)) return 0;

    hwnd=CreateWindow(lpClassName,lpClassTitle,WS_OVERLAPPEDWINDOW
                      ,CW_USEDEFAULT,CW_USEDEFAULT,800,400,
                      //��s�Ō�2�̃f�t�H�[���g�l�́@CW_USEDEFAULT,CW_USEDEFAULT,
                      HWND_DESKTOP,NULL,hInstance,NULL);
    if(hwnd==NULL) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while((bRet=GetMessage(&msg,NULL,0,0))!=0) {
        if(bRet==-1) break;
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
/* ----------------------------------------------------------- */
int sine_func(short *data,int strt_count,int n_data,
              int fs,double amp, double f)
{
    int i,j;
    double cons;
    cons=2.*pi*f/(double)fs;
    j=strt_count;
    for(i=0; i<n_data; i++)
        data[i]=(short)(amp*sin(cons*(double)j++ ));
    return j;
}