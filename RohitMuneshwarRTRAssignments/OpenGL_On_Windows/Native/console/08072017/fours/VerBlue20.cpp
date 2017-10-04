#include<Windows.h>
#include<gl/GL.h>

#pragma comment(lib, "opengl32.lib")

#define WIN_WIDTH 600
#define WIN_HEIGHT 600

bool gbActiveWindow = false;
bool gbEscapePressed = false;
bool gbFullScreen = false;
DWORD dwStyle;
HWND ghwnd;
HDC ghdc;
HGLRC ghrc;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);
	WNDCLASSEX wndclass;
	TCHAR szAppName[] = TEXT("My App");
	HWND hwnd;
	bool gbDone = false;
	MSG msg;
	RECT rect;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
	RegisterClassEx(&wndclass);
	
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
	int x = (rect.right - rect.left)/2 - WIN_WIDTH/2;
	int y = (rect.bottom - rect.top)/2 - WIN_HEIGHT/2;
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("20 equally spaced blue lines to left and right of GreenLine"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		x,y,WIN_WIDTH, WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ghwnd = hwnd;
	ShowWindow(hwnd, iCmdShow);
	//UpdateWindow(hwnd);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	initialize();
	while(gbDone == false){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			if(msg.message == WM_QUIT){
				gbDone = true;
			}else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}else{
			if(gbActiveWindow == true){
				if(gbEscapePressed == true){
					gbDone = true;
				}
			}
			display();
		}
	}
	uninitialize();
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
	void ToggleFullscreen(void);
	void resize(int,int);
	
	switch(iMsg){
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0){
				gbActiveWindow = true;
			}else{
				gbActiveWindow = false;
			}
		break;
		
		case WM_SIZE:
			resize(LOWORD(lParam),HIWORD(lParam));
		break;
		
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					PostQuitMessage(0);
				break;
				case 0x46:
					ToggleFullscreen();
				break;
			}
		break;
		
	}
	return DefWindowProc(hwnd,iMsg,wParam,lParam);
}

void resize(int width, int height){
	if(height == 0){
		height = 1;
	}
	glViewport(0,0,(GLsizei) width, (GLsizei) height);
}

void ToggleFullscreen(){
	MONITORINFO mi;
	if(gbFullScreen == false){
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW){
		mi= {sizeof(MONITORINFO)};
			bool fWndPlacement = GetWindowPlacement(ghwnd, &wpPrev);
			bool fMonitorInfo = GetMonitorInfo(MonitorFromWindow(ghwnd, MONITOR_DEFAULTTOPRIMARY),&mi);
			
			if(fWndPlacement && fMonitorInfo){
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom-mi.rcMonitor.top,
				SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}else{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

void initialize(){
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	
	ghdc = GetDC(ghwnd);
	int iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0){
		ReleaseDC(ghwnd,ghdc);
		ghdc = NULL;
	}
	
	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd)==FALSE){
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	ghrc = wglCreateContext(ghdc);
	if(ghrc == NULL){
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	
	if(wglMakeCurrent(ghdc,ghrc)==FALSE){
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
}

void uninitialize(){
	if(gbFullScreen == true){
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
	
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc=NULL;
	ReleaseDC(ghwnd,ghdc);
	ghdc=NULL;
	DestroyWindow(ghwnd);
	ghwnd= NULL;
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//20 equally spaced, full length BLUE colored vertical lines, each of width 1, to the left and similarly, 20 to the right of the vertical GREEN line  (The GREEN line from
	
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,-1.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glEnd();
	
	glLineWidth(1.0f);
	for(float i=0.0;i<=1.0;i=i+0.05){
		glBegin(GL_LINES);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(i,-1.0f,0.0f);
		glVertex3f(i,1.0f,0.0f);
		glEnd();
	}
	for(float i=-1.0;i<=0.0;i=i+0.05){
		glBegin(GL_LINES);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(i,-1.0f,0.0f);
		glVertex3f(i,1.0f,0.0f);
		glEnd();
	}
	
	SwapBuffers(ghdc);
}