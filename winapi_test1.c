#include <windows.h>
#define BUTTON_ID1 0
#define BUTTON_ID2 1
int Mode;
WNDPROC DefStaticProc;

LRESULT CALLBACK StaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  switch(msg) {
  case WM_NCHITTEST:
    if (GetKeyState(VK_LBUTTON) < 0)
      MessageBox(
        hwnd , TEXT("window0") ,
        TEXT("Area0") , MB_OK
      );
    return 0;
  }
  return CallWindowProc(DefStaticProc , hwnd , msg , wp , lp);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  HDC hdc;
  PAINTSTRUCT ps;
  static RECT rct;
  static BOOL blMouse = FALSE;
  static HWND rect_box;
  static unsigned short int x = -10 , y = -10;


  switch (msg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_LBUTTONDOWN:
    blMouse = TRUE;
    rct.left = LOWORD(lp);
    rct.top = HIWORD(lp);
    SetCapture(hwnd);
    return 0;
  case WM_LBUTTONUP:
    blMouse = FALSE;
    ReleaseCapture();
    return 0;
  case WM_MOUSEMOVE:
    if(Mode==0) {
      if(blMouse) {
        rct.right = LOWORD(lp);
        rct.bottom = HIWORD(lp);
        InvalidateRect(hwnd , NULL , TRUE);
      }
    } else if (Mode==1) {
      if(wp & MK_LBUTTON) {
        x = LOWORD(lp);
        y = HIWORD(lp);
        InvalidateRect(hwnd , NULL , FALSE);
      }
    }
    return 0;
  case WM_PAINT:
    hdc = BeginPaint(hwnd , &ps);
    if(Mode==0)Rectangle(hdc, rct.left, rct.top, rct.right, 
                         rct.bottom);
    if(Mode==1) {
      SelectObject(hdc , GetStockObject(BLACK_BRUSH));
      Ellipse(hdc , x - 2 , y - 2 , x + 2 , y + 2);
    }
    EndPaint(hwnd , &ps);
    return 0;
  case WM_CREATE:
    rect_box = CreateWindow(
                 TEXT("STATIC") , TEXT("Area0") ,
                 WS_CHILD | WS_VISIBLE ,
                 300 , 0 , 200 , 45 ,
                 hwnd , (HMENU)1 ,
                 ((LPCREATESTRUCT)(lp))->hInstance , NULL
               );
    DefStaticProc = (WNDPROC)GetWindowLong(rect_box , GWL_WNDPROC);
    SetWindowLong(rect_box , GWL_WNDPROC , (LONG)StaticProc);
    return 0;
  case WM_COMMAND:
    switch(LOWORD(wp)) {
    case BUTTON_ID1:
      MessageBox(hwnd, TEXT("Rubber band"), TEXT("Mode 0"), MB_OK);
      Mode = 0;
      break;
    case BUTTON_ID2:
      MessageBox(hwnd, TEXT("Drawing dots"), TEXT("Mode 1"), MB_OK);
      Mode = 1;
      break;
    }
    return 0;

  }
  return DefWindowProc(hwnd , msg , wp , lp);
}

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance ,
                   PSTR lpCmdLine , int nCmdShow )
{
  HWND hwnd;
  MSG msg;
  WNDCLASS winc;

  winc.style = CS_HREDRAW | CS_VREDRAW;
  winc.lpfnWndProc = WndProc;
  winc.cbClsExtra = winc.cbWndExtra = 0;
  winc.hInstance = hInstance;
  winc.hIcon = LoadIcon(NULL , IDI_APPLICATION);
  winc.hCursor = LoadCursor(NULL , IDC_ARROW);
  winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  winc.lpszMenuName = NULL;
  winc.lpszClassName = TEXT("Window_class1");
 
  if (!RegisterClass(&winc)) return -1;

  hwnd = CreateWindow(
           TEXT("Window_class1") , TEXT("Basic Window") ,
           WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
           300,300,800,600, // Coordinate of main window
           NULL , NULL , hInstance , NULL
         );

  CreateWindow(
    TEXT("BUTTON") , TEXT("Mode 0") ,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON ,
    0 , 0 , 100 , 50 ,
    hwnd , (HMENU)BUTTON_ID1 , hInstance , NULL
  );

  CreateWindow(
    TEXT("BUTTON") , TEXT("Mode 1") ,
    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON ,
    0 , 50 , 100 , 50 ,
    hwnd , (HMENU)BUTTON_ID2 , hInstance , NULL
  );
  if (hwnd == NULL) return -1;

  while(GetMessage(&msg , NULL , 0 , 0)) DispatchMessage(&msg);
  return msg.wParam;
}
