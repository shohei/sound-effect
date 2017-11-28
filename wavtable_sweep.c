#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define ID_TRACK   100
#define ID_STATIC  101

int fs=44100; //サンプリング周波数　(Hz);
double pi=3.14159265358979323846;

double f=357.43 ; // 信号の周波数(Hz)
double cons1,cons2;
static double djf;
static int idf,idtempf;
int rdpara, n_data;
double amp=10000.;
static int ninterp;
int sine_func(short *data,int strt_count,int n_data, int fs,double amp, double f, int id);
static int nbuf=2048;
static double sintbl[2048];
static int s_k, s_k1;

HWND hTrack,hStatic;
HWND CreateControlWindow(HWND hwndParent, int Left, int Top,int Width, int Height,
                         int dwExStyle ,int dwFlag ,LPCTSTR Caption, LPCTSTR ClassName,
                         HMENU ChildID,HINSTANCE hInstance);

static int s_stop;
#define kNumBuffers (4)
LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    WAVEFORMATEX wfe;
    static HWAVEOUT hWaveOut;
    static WAVEHDR whdr[kNumBuffers];
    static short  *lpWave[kNumBuffers];
    int i,k,pos,idur=1,idiv=10;
    static int k0=1;

    double d;

    static int buf_num;
    HDC hdc;
    PAINTSTRUCT ps;
    char str[128],str1[128],cbuf[100];
    MMTIME mmt;
    static int ms;

    switch(uMsg) {
    case WM_CREATE:
        wfe.wFormatTag=WAVE_FORMAT_PCM;
        wfe.nChannels=1;	//モノラル
        wfe.wBitsPerSample=16;	//量子化ビット数
        wfe.nBlockAlign=wfe.nChannels * wfe.wBitsPerSample/8;
        wfe.nSamplesPerSec=fs;	//標本化周波数
        wfe.nAvgBytesPerSec=wfe.nSamplesPerSec * wfe.nBlockAlign;

        waveOutOpen(&hWaveOut,WAVE_MAPPER,&wfe,(DWORD)hWnd,0,CALLBACK_WINDOW);

        n_data=fs*idur/idiv; // 音波形のバッファサイズ
        cons1=fs/f;
        cons2=f/fs;
        for(k=0; k<kNumBuffers; k++) {
            lpWave[k]=(short *)calloc(wfe.nAvgBytesPerSec/idiv,idur); //idur 秒分
            rdpara=sine_func(lpWave[k],rdpara,n_data, fs,amp, f, 0); //波形データ作成
            whdr[k].lpData=(LPSTR)lpWave[k];
            whdr[k].dwBufferLength=wfe.nAvgBytesPerSec/idiv * idur;
            whdr[k].dwFlags=WHDR_BEGINLOOP | WHDR_ENDLOOP;
            whdr[k].dwLoops=1;
            waveOutPrepareHeader(hWaveOut,&whdr[k],sizeof(WAVEHDR));
        }

        hStatic=CreateControlWindow(hWnd,10,110,250,45,0,
                                    0    ,"","STATIC",(HMENU)ID_STATIC,
                                    (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE));
        InitCommonControls(); //コモンコントロールの初期化
//トラックバーコントロール
        hTrack=CreateControlWindow(hWnd,10,160,580,55,0 ,
                                   TBS_AUTOTICKS,"",TRACKBAR_CLASS,(HMENU)ID_TRACK,
                                   (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE));
        SendMessage(hTrack,TBM_SETRANGE,TRUE,MAKELONG(0, 600));//トラックバーの範囲
        SendMessage(hTrack,TBM_SETPAGESIZE,0,10);//マウスクリック時の移動量
        SendMessage(hTrack,TBM_SETLINESIZE,0,1);//矢印キーを押した時の移動量
        SendMessage(hTrack,TBM_SETPOS,TRUE,100);//ポジション
        SendMessage(hTrack,TBM_SETTICFREQ ,20,0);//目盛りのサイズ
        SetFocus(hTrack);
        return 0;
    case WM_LBUTTONDOWN:
        for(k=0; k<kNumBuffers; k++) {
            waveOutWrite(hWaveOut,&whdr[k],sizeof(WAVEHDR));
            whdr[k].dwUser=0;
            buf_num=k;
        }
        return 0;
    case WM_RBUTTONDOWN:
        waveOutReset(hWaveOut);
        for(k=0; k<kNumBuffers; k++) {
            whdr[k].dwUser=1; //waveOutReset関数呼び出し
        }
        return 0;
    case MM_WOM_DONE:
        if(((LPWAVEHDR)lParam)->dwUser) { //waveOutReset関数で停止
            buf_num=0;
        } else { //最後まで再生
            waveOutWrite((HWAVEOUT)wParam,(LPWAVEHDR)lParam,sizeof(WAVEHDR));
            for(k=0; k<kNumBuffers; k++) {
                if((LPWAVEHDR)lParam==&whdr[k]) {
                    buf_num=k; //再生を終了したバッファ
                    k0++;
                    rdpara=sine_func(lpWave[buf_num],rdpara,n_data, fs,amp, f,1);
                    break;
                }
            }
            buf_num=(buf_num+1)%kNumBuffers; //再生中のバッファ
        }
        //InvalidateRect(hWnd,NULL,TRUE);
        return 0;
    case WM_HSCROLL: // added for TrackBar
        if ((HWND)(lParam)==hTrack) {
            wsprintf(cbuf,"%d [Hz]",pos=SendMessage(hTrack,TBM_GETPOS,0,0));
            SendMessage(hStatic,WM_SETTEXT,0,(LPARAM)cbuf);
            if (f != pos) {
                f=(double)pos;
                if(f==0) f=0.0001;
                cons1=fs/f;
                cons2=f/fs;
                djf=0.;
                idf=idtempf=0;
                s_k1 = s_k;
            }
        }
        return(DefWindowProc(hWnd, uMsg, wParam, lParam));
    case WM_DESTROY:
        waveOutReset(hWaveOut);
        for(k=0; k<kNumBuffers; k++) {
            waveOutUnprepareHeader(hWaveOut,&whdr[k],sizeof(WAVEHDR));
        }
        waveOutClose(hWaveOut);
        for(k=0; k<kNumBuffers; k++) {
            free(lpWave[k]);
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow)
{
    HWND hWnd;
    WNDCLASS wc;
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
    wc.lpszClassName = __FILE__;
    if(!RegisterClass(&wc)) return 0;
    rdpara=0;
    hWnd=CreateWindow(__FILE__,"波形テーブル合成",
                      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                      CW_USEDEFAULT,CW_USEDEFAULT,
                      610,400,
                      NULL,NULL,hInstance,NULL);
    if(hWnd==NULL) return 0;

    while((bRet=GetMessage(&msg,NULL,0,0))!=0) {
        if(bRet==-1) break;
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
//------------------------------------------------------
HWND CreateControlWindow(HWND hwndParent, int Left, int Top,int Width,
                         int Height,int dwExStyle ,int dwFlag ,LPCTSTR Caption,
                         LPCTSTR ClassName,HMENU ChildID,HINSTANCE hInstance)
{
    return CreateWindowEx(dwExStyle,ClassName,Caption,WS_CHILD | WS_VISIBLE | dwFlag    ,
                          Left, Top, Width, Height, hwndParent, ChildID, hInstance, NULL);
}

static double lastUsedf,lastCons1,lastCons2;
int sine_func(short *data,int strt_count,int n_data, int fs,
              double amp, double f, int id)
{
    int i,j;
    double cons,a,a1,p,p1,w,w1,ftemp,temp,temp1;
    double lastf,lastc1,lastc2;

    lastf = lastUsedf;
    lastc1= lastCons1;
    lastc2= lastCons2;
    cons=2.*pi/(double)nbuf;

    if(id==0) {
        for (i=0; i<nbuf; i++) {
            sintbl[i]=sin(cons*(double)i);
        }
        return strt_count;
    } else {
        j=strt_count;
        for(i=0; i<n_data; i++) {
            w = cons2*fmod((double)j,cons1);  //  鋸波 (phasor)
            p= (double)nbuf*w;
            temp= amp*( (1.- w ) * sintbl[(s_k=(int)p)]
                        + w*sintbl[s_k==(nbuf-1)? 0 : s_k+1]);
            if(idf==1) {
                data[i]=(short)temp;

            } else { // idf==0
                w1 =    lastc2*fmod((double)j,lastc1);  //  鋸波 (phasor)
                p1=(double)nbuf*w1;
                temp1= amp*( (1.- w1 ) * sintbl[(s_k1=(int)p1)]
                             + w1*sintbl[s_k1==(nbuf-1)? 0 : (s_k1+1)]);
                data[i]=(short)((1.0 - djf) * temp1 + djf * temp);
                if(djf>=1.)idf=1;
                djf +=0.0008;
            }   // end of else (idf==0)
            j++;
        } // end of for i
        lastUsedf = f;
        lastCons1=cons1;
        lastCons2=cons2;
        return j;
    }
}
