#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define N_BUFFER 2 //マルチバッファ数（通常２）

int    fs=44100;    //サンプリング周波数　(Hz);
double f =357.43 ;  // 信号の周波数(Hz)
double amp=10000.;
double pi=3.14159265358979323846;

int rdpara, n_data;

int sine_func(short *data,int strt_count,int n_data, int fs,double amp, double f);

static LPSTR lpClassName = "My Class";
static LPSTR lpClassTitle = "Music Keyboard for Harmonics";

// GUI 関連
static int nRect=12, width=50;
static int iRect=0,lRect=0; // 現在のRect#，　直前のRect# 0:範囲外
static int Isplaying=0;  //0: 演奏停止中(=closed), 1: 初期化 2: 演奏中
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
        wfe.nChannels=1;	//モノラル
        wfe.wBitsPerSample=16;	//量子化ビット数
        wfe.nBlockAlign=wfe.nChannels * wfe.wBitsPerSample/8;
        wfe.nSamplesPerSec=fs;	//標本化周波数
        wfe.nAvgBytesPerSec=wfe.nSamplesPerSec * wfe.nBlockAlign;

        waveOutOpen(&hWaveOut,WAVE_MAPPER,&wfe,(DWORD)hwnd,0,CALLBACK_WINDOW);

        rdpara=0;
        n_data=fs*idur;
        for(k=0; k<N_BUFFER; k++) {
            lpWave[k]=(short *)calloc(wfe.nAvgBytesPerSec,idur);	//idur 秒分
            rdpara=sine_func(lpWave[k],rdpara,n_data, fs,amp, f);	//波形データ作成
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
                iRect=i+1; // iRect 番号（鍵盤の左からの番号)は1始まり
                break;  // iRect確定
            }
        }

        if (iRect!=0 && (lRect==iRect ||  lRect==0 || lRect!=0) ) { //鍵盤上
            //マルチバッファリングをするために空データを書き込み
            if(lRect!=0) { // 現在は鍵盤上かつ直前も鍵盤上
                if (Isplaying>0) {
                    Isplaying=0;
                    waveOutReset((HWAVEOUT)hWaveOut); // 現在の発音を中止
                    //  ただし　whdr[i].dwUser=1;は行わない
                    IDMouseUse=0;
                }
            } else { // 現在は鍵盤上、直前は鍵盤外  IDMouseUse==0

                for(k=0; k<N_BUFFER; k++) {
                    waveOutWrite(hWaveOut,&whdr[k],sizeof(WAVEHDR));
                    whdr[k].dwUser=0;
                    buf_num=k;
                }
            }
            Isplaying=1; // 初期化
            IDMouseUse=1; // DA用にマウス使用
        } else if (Isplaying>0) { // 鍵盤外　DA停止
            Isplaying=0;

            waveOutReset(hWaveOut);
            for(k=0; k<N_BUFFER; k++)whdr[k].dwUser=1; //waveOutReset関数の呼び出し
            IDMouseUse=0;

        }
        return 0;
    case WM_RBUTTONDOWN:
        waveOutReset(hWaveOut);
        for(k=0; k<N_BUFFER; k++)whdr[k].dwUser=1; //waveOutReset関数を呼び出し
        return 0;

    case MM_WOM_DONE:
        if(((LPWAVEHDR)lParam)->dwUser) {	//waveOutReset関数での停止を受けて
            buf_num=0;		//初期値に戻す
        } else {		//最後まで再生した
            waveOutWrite((HWAVEOUT)wParam,(LPWAVEHDR)lParam,sizeof(WAVEHDR));
            for(k=0; k<N_BUFFER; k++) {
                if((LPWAVEHDR)lParam==&whdr[k]) {
                    buf_num=k;		//再生を終了したバッファ
                    rdpara=sine_func(lpWave[buf_num],rdpara,n_data, fs,amp, f*iRect);
                    break;
                }
            }
            buf_num=(buf_num+1)%N_BUFFER;  //再生中のバッファ

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
                      //上行最後2つのデフォールト値は　CW_USEDEFAULT,CW_USEDEFAULT,
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