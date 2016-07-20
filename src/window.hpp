#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <windows.h>
#include "gl\gl.h"
#include "gl\glu.h"

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)    ((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)    ((int)(short)HIWORD(lParam))
#endif

typedef LRESULT CALLBACK (*WndProcCallBack_Ptr)(HWND,UINT,WPARAM,LPARAM);

class Window
{
public:
    HINSTANCE   hInstance;
    HDC         hDC;
    HGLRC       hRC;
    HWND        hWnd;
    
    int width, height;
    int pixel_width,pixel_height;
    WndProcCallBack_Ptr WndProc;
    
    Window(int w,int h,float enlarge,WndProcCallBack_Ptr wndproc);
    ~Window(){};
    int SetTitle(char * title);
    int Create();
    int Destroy();
};

#endif
