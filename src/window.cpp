#include "window.hpp"
#include <stdio.h>

static const PIXELFORMATDESCRIPTOR pfd =
{
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    32,
    0, 0, 0, 0, 0, 0, 8, 0,
    0, 0, 0, 0, 0,  
    32,             
    0,              
    0,              
    PFD_MAIN_PLANE,
    0, 0, 0, 0
};

char wndclass[4]="-";

Window::Window(int w,int h,float enlarge,WndProcCallBack_Ptr wndproc)
{
    width=(int)(w*enlarge);
    height=(int)(h*enlarge);
    pixel_width=w;
    pixel_height=h;
    WndProc=wndproc;
}

int Window::Create()
{
    hInstance=0;
    hDC=0;
    hRC=0;
    hWnd=0;
    
    unsigned int PixelFormat;
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES;
    DWORD dwStyle = WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
    
    RECT rec;
    WNDCLASS wc;

    ZeroMemory( &wc, sizeof(WNDCLASS) );
    wc.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = wndclass;

    if( !RegisterClass(&wc) )
        return( 0 );

    rec.left   = 0;
    rec.top    = 0;
    rec.right  = width;
    rec.bottom = height;
    AdjustWindowRect( &rec, dwStyle, 0 );

    hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, " ", dwStyle,
                           (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
                           (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
                           rec.right-rec.left, rec.bottom-rec.top, 0, 0, hInstance, 0 );
    if( !hWnd )
    {
        return( 0 );
    }
    if( !(hDC=GetDC(hWnd)) )
        return( 0 );
    if( !(PixelFormat=ChoosePixelFormat(hDC,&pfd)) )
        return( 0 );
    if( !SetPixelFormat(hDC,PixelFormat,&pfd) )
        return( 0 );
    if( !(hRC=wglCreateContext(hDC)) )
        return( 0 );
    if( !wglMakeCurrent(hDC,hRC) )
        return( 0 );
    
    return( 1 );
}

int Window::Destroy()
{
    if( hRC )
    {
        wglMakeCurrent( 0, 0 );
        wglDeleteContext( hRC );
    }
    if( hDC  ) ReleaseDC( hWnd, hDC );
    if( hWnd ) DestroyWindow( hWnd );
    UnregisterClass( wndclass, hInstance );
    return 0;
}

int Window::SetTitle(char * title)
{
    SetWindowTextA(hWnd, title);
    return 0;
}
