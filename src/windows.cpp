// Optional native Windows host. The same C++ engine runs without a browser or JS.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <sapi.h>
#include <cstdio>
#include <cstring>
#include "game.cpp"

static ISpVoice* speech=nullptr;
static u32 dib[MAX_WIDTH*MAX_HEIGHT];
static char saveDir[MAX_PATH]={};
static LARGE_INTEGER frequency,lastTick;
static void savePath(char* path,const char* file){std::snprintf(path,MAX_PATH,"%s\\%s",saveDir,file);}
extern "C" int host_progress(){char path[MAX_PATH];savePath(path,"progress.txt");auto f=std::fopen(path,"rb");if(!f)return 0;int p=0;std::fscanf(f,"%d",&p);std::fclose(f);return p&PROGRESS_MASK;}
extern "C" int host_save_progress(int p){char path[MAX_PATH];savePath(path,"progress.txt");auto f=std::fopen(path,"wb");if(!f)return 0;bool ok=std::fprintf(f,"%d",p&PROGRESS_MASK)>0;return std::fclose(f)==0&&ok;}
extern "C" void host_voice(int id){if(!speech)return;speech->Speak(nullptr,SPF_ASYNC|SPF_PURGEBEFORESPEAK,nullptr);static const wchar_t* lines[]={L"",L"Ronaldo is not in my plans.",L"Siuuuuuu!",L"So... who is the greatest?",L"I am number one, two and three in history.",L"The World Cup is not my dream."};if(id>0&&id<6){speech->SetRate(id==2?-3:0);speech->Speak(lines[id],SPF_ASYNC,nullptr);}}
extern "C" void host_tone(int,int){} // Native speech is supported; browser host also provides synth effects.
static RECT viewport(HWND window){RECT rc;GetClientRect(window,&rc);int w=rc.right,h=rc.bottom;int vw=w,vh=w*HEIGHT/WIDTH;if(vh>h){vh=h;vw=h*WIDTH/HEIGHT;}return{(w-vw)/2,(h-vh)/2,(w+vw)/2,(h+vh)/2};}
static void pointer(HWND window,int type,LPARAM lp){RECT v=viewport(window);int w=v.right-v.left,h=v.bottom-v.top;if(!w||!h)return;game_pointer(type,(short(LOWORD(lp))-v.left)*WIDTH/w,(short(HIWORD(lp))-v.top)*HEIGHT/h);}
static LRESULT CALLBACK windowProc(HWND window,UINT msg,WPARAM wp,LPARAM lp){
 switch(msg){
 case WM_SIZE:game_resize(LOWORD(lp),HIWORD(lp));return 0;
 case WM_TIMER:{LARGE_INTEGER now;QueryPerformanceCounter(&now);float dt=float(double(now.QuadPart-lastTick.QuadPart)/frequency.QuadPart);lastTick=now;game_tick(dt);InvalidateRect(window,nullptr,FALSE);return 0;}
 case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(window,&ps);RECT rc;GetClientRect(window,&rc);FillRect(dc,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));RECT v=viewport(window);for(int i=0;i<WIDTH*HEIGHT;i++){u32 c=game.r.pixels[i];dib[i]=(c&0xff00ff00u)|((c&255)<<16)|((c>>16)&255);}BITMAPINFO info={};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=WIDTH;info.bmiHeader.biHeight=-HEIGHT;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;SetStretchBltMode(dc,COLORONCOLOR);StretchDIBits(dc,v.left,v.top,v.right-v.left,v.bottom-v.top,0,0,WIDTH,HEIGHT,dib,&info,DIB_RGB_COLORS,SRCCOPY);EndPaint(window,&ps);return 0;}
 case WM_KEYDOWN:if(wp<256)game_key(int(wp),1);return 0;
 case WM_KEYUP:if(wp<256)game_key(int(wp),0);return 0;
 case WM_LBUTTONDOWN:SetFocus(window);SetCapture(window);pointer(window,0,lp);return 0;
 case WM_MOUSEMOVE:pointer(window,1,lp);return 0;
 case WM_LBUTTONUP:pointer(window,2,lp);ReleaseCapture();return 0;
 case WM_KILLFOCUS:game_blur();return 0;
 case WM_DESTROY:KillTimer(window,1);PostQuitMessage(0);return 0;
 }return DefWindowProcW(window,msg,wp,lp);
}
int WINAPI WinMain(HINSTANCE instance,HINSTANCE,LPSTR,int show){
 SetProcessDPIAware();char base[MAX_PATH];DWORD count=GetEnvironmentVariableA("LOCALAPPDATA",base,MAX_PATH);if(count&&count<MAX_PATH-20){std::snprintf(saveDir,MAX_PATH,"%s\\DesertGoat",base);CreateDirectoryA(saveDir,nullptr);}else std::strcpy(saveDir,".");
 HRESULT com=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);if(SUCCEEDED(com))CoCreateInstance(CLSID_SpVoice,nullptr,CLSCTX_ALL,IID_ISpVoice,(void**)&speech);
 game_init();WNDCLASSW wc={};wc.lpfnWndProc=windowProc;wc.hInstance=instance;wc.lpszClassName=L"DesertGoatCpp";wc.hCursor=LoadCursor(nullptr,IDC_ARROW);RegisterClassW(&wc);
 RECT bounds={0,0,WIDTH,HEIGHT};AdjustWindowRect(&bounds,WS_OVERLAPPEDWINDOW,FALSE);HWND window=CreateWindowW(wc.lpszClassName,L"Desert Goat: SIUUUUU - C++",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,bounds.right-bounds.left,bounds.bottom-bounds.top,nullptr,nullptr,instance,nullptr);if(!window)return 1;
 QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&lastTick);SetTimer(window,1,16,nullptr);ShowWindow(window,show);MSG message;while(GetMessage(&message,nullptr,0,0)>0){TranslateMessage(&message);DispatchMessage(&message);}if(speech){speech->Speak(nullptr,SPF_ASYNC|SPF_PURGEBEFORESPEAK,nullptr);speech->Release();}if(SUCCEEDED(com))CoUninitialize();return 0;
}
