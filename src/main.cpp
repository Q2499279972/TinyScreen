#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <string.h>
#include <stdio.h>

#include "window.hpp"
#include "ReadLuaConfig.hpp"
#include "debug.hpp"

Window * ScreenWindow;
static int running;
int xres;
int yres;
float enlarge;
LuaConfig settings;
unsigned int *sharedBuffer;

void ExecLua(char * filename);
void CreateConsole();
unsigned int* CreateSharedMemory(const TCHAR* szName,int size);
static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void DrawPoint(int x,int y, unsigned int color);
int LuaDrawPoint(lua_State *L);
int LuaGetWidth(lua_State *L);
int LuaGetHeight(lua_State *L);
void ExecLua(char * filename);



void CreateConsole()
{
    if( !AllocConsole() ) MessageBox(NULL, TEXT("create console failed!"), NULL, 0);
    SetConsoleTitle(TEXT("stdout"));
    freopen(TEXT("CONOUT$"),TEXT("w"), stdout);
    HWND hwnd = GetConsoleWindow();
    HMENU hMenu = GetSystemMenu(hwnd,false);
    DeleteMenu(hMenu,SC_CLOSE,MF_BYCOMMAND);
}

unsigned int* CreateSharedMemory(const TCHAR* szName,int size)
{
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,size,szName);
    if (hMapFile == NULL)
    {
        DBGPRINT("CreateFileMapping error\n");
        return NULL;
    }
    unsigned int * sharedBuffer;
    sharedBuffer = (unsigned int * ) MapViewOfFile(hMapFile,FILE_MAP_WRITE,0,0,size);
    memset(sharedBuffer,0,size);
    return sharedBuffer;
}

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch (uMsg)
    {
        case WM_DROPFILES:
        {
            UINT count;
            HDROP hDropInfo = (HDROP)wParam;
            count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
            if( count==1 )
            {
                //however, it may be better to run it in a new thread! 
                char filePath[256];
                DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath));
                DragFinish(hDropInfo);
                DBGPRINT("Drag in file %s\n",filePath);
                DBGPRINT("start execute script\n");
                ExecLua(filePath);
                DBGPRINT("finished execute script\n");
            }
            break;
        }
        case WM_CONTEXTMENU:
        {
            POINT p = {LOWORD(lParam),HIWORD(lParam)};
            HMENU hMainMenu = GetMenu(hWnd);
            HMENU m = CreatePopupMenu();
            AppendMenu(m, MF_STRING, 0x8001,"Clear");
            AppendMenu(m, MF_STRING, 0x8002,"Export to bmp file");
            AppendMenu(hMainMenu, MF_STRING | MF_POPUP, (UINT)m, "main");
            TrackPopupMenu(m, TPM_VERTICAL|TPM_TOPALIGN, p.x, p.y, 0, hWnd, NULL);
            DestroyMenu(m);
            break;
        }
        case WM_COMMAND:
        {
            switch (wParam)
            {
                case 0x8001:
                {
                    memset(sharedBuffer,0,xres*yres*4);
                    break;
                }
                case 0x8002:
                {
                    SYSTEMTIME tm;
                    GetLocalTime(&tm);
                    
                    char bmpName[100];
                    snprintf(bmpName,100,"bmpsave_%4.4d%2.2d%2.2d_%2.2dh%2.2dm%2.2ds.bmp",
                            tm.wYear, 
                            tm.wMonth+1, 
                            tm.wDay,  
                            tm.wHour, 
                            tm.wMinute,
                            tm.wSecond);
                            
                    FILE *fp=fopen(bmpName,"wb+");
                    if(fp==NULL)
                        break;
                    
                    int width=xres;
                    int height=yres;
                    
                    int biBitCount=32;
                    int lineByte=(width * biBitCount/8+3)/4*4;
                    int colorTablesize = 0;
                                       
                    BITMAPFILEHEADER fileHead;
                    fileHead.bfType = 0x4D42;
                    fileHead.bfSize= sizeof(BITMAPFILEHEADER) + 
                                     sizeof(BITMAPINFOHEADER) + 
                                     colorTablesize + 
                                     lineByte*height;
                    fileHead.bfReserved1 = 0;
                    fileHead.bfReserved2 = 0;
                    fileHead.bfOffBits=54+colorTablesize;
                    
                    BITMAPINFOHEADER head;
                    head.biBitCount=biBitCount;
                    head.biClrImportant=0;
                    head.biClrUsed=0;
                    head.biCompression=0;
                    head.biHeight=-height;
                    head.biPlanes=1;
                    head.biSize=40;
                    head.biSizeImage=lineByte*height;
                    head.biWidth=width;
                    head.biXPelsPerMeter=0;
                    head.biYPelsPerMeter=0;
                    
                    fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);
                    fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);
                    fwrite(sharedBuffer, height*lineByte, 1, fp);
                    
                    fclose(fp);
                }
            }
            break;
        }
        case WM_CLOSE: 
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return( 0 );
        }
    }
    return( DefWindowProc(hWnd,uMsg,wParam,lParam) );
}

void DrawPoint(int x,int y, unsigned int color)
{
    if(x<0 || y<0 || x>xres-1 || y>yres-1)
    {
        return;
    }
    else
    {
        sharedBuffer[x+y*xres] = color;
    }
}

int LuaDrawPoint(lua_State *L)
{
    int x,y;
    unsigned int r,g,b;
    x=(int)luaL_checknumber(L, 1);
    y=(int)luaL_checknumber(L, 2);
    r=(unsigned int)luaL_checknumber(L, 3);
    g=(unsigned int)luaL_checknumber(L, 4);
    b=(unsigned int)luaL_checknumber(L, 5);
    if(r>255) r=255;
    if(g>255) g=255;
    if(b>255) b=255;
    DrawPoint(x,y,(b)|(g<<8)|(r<<16));
    lua_pushnumber(L,0);
    return 1;
}

int LuaGetWidth(lua_State *L)
{
    lua_pushnumber(L,xres);
    return 1;
}

int LuaGetHeight(lua_State *L)
{
    lua_pushnumber(L,yres);
    return 1;
}

void ExecLua(char * filename)
{
    lua_State *luastat;
    luastat = luaL_newstate();
    luaL_openlibs(luastat);
    
    lua_register(luastat, "DrawPoint", LuaDrawPoint);
    lua_register(luastat, "GetWidth", LuaGetWidth);
    lua_register(luastat, "GetHeight", LuaGetHeight);
    
    int error = luaL_dofile(luastat, filename);
    if(error)
    {
        const char *message = lua_tostring(luastat, -1);
        printf("LUA ERROR: %s\n", message);
        lua_pop(luastat, 1);
    }
    lua_close(luastat);
}

int WINAPI WinMain( HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

    CreateConsole();
    
    ReadLuaConfig(&settings,"./config.lua");
    
    xres = settings.width;
    yres = settings.height;
    enlarge = settings.factor;
    
    sharedBuffer = CreateSharedMemory(TEXT("Global\\MemoryMappingObject"),xres*yres*4);
    ScreenWindow = new Window(xres,yres,enlarge,WndProc);
    
    running = 1;
 
    ScreenWindow->Create();
    char title[100];
    snprintf(title,100,"screen x:%dy:%d X%f",xres,yres,enlarge);
    ScreenWindow->SetTitle(title);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    while(running)
    {
        glClearColor( 0.2f, 0.4f, 1.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        glRasterPos2i(-1,1);
        glPixelZoom(enlarge, -enlarge);
        glDrawPixels(xres, yres, GL_BGRA, GL_UNSIGNED_BYTE, sharedBuffer);
        
        MSG msg;
        while( PeekMessage(&msg,0,0,0,PM_REMOVE) )
        {
            if( msg.message==WM_QUIT ) running=0;
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

        SwapBuffers( ScreenWindow->hDC );
        Sleep(50);
    }
    
    ScreenWindow->Destroy();
    delete ScreenWindow;
    return( 0 );
}



