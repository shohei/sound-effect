#include <windows.h>
WNDPROC DefStaticProc;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, 
                         LPARAM lp)
{
  HDC hdc;
  PAINTSTRUCT ps;

  switch (msg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine , int nCmdShow )
{
  HWND hwnd;
  MSG msg;
  WNDCLASS winc;

  winc.style         = CS_HREDRAW | CS_VREDRAW;
  winc.lpfnWndProc   = WndProc;
  winc.cbClsExtra    = winc.cbWndExtra = 0;
  winc.hInstance     = hInstance;
  winc.hIcon         = LoadIcon(NULL , IDI_APPLICATION);
  winc.hCursor       = LoadCursor(NULL , IDC_ARROW);
  winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  winc.lpszMenuName  = NULL;
  winc.lpszClassName = TEXT("Window_class1");

  if (!RegisterClass(&winc)) return -1;

  hwnd = CreateWindow(
           TEXT("Window_class1") , TEXT("Basic Window") ,
           WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
           CW_USEDEFAULT , CW_USEDEFAULT ,
           CW_USEDEFAULT , CW_USEDEFAULT ,
           NULL , NULL , hInstance , NULL
         );

  if (hwnd == NULL) return -1;

  while(GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
  return msg.wParam;
}
