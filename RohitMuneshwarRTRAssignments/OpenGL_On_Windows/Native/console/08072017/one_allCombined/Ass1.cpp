#include<Windows.h>
#include<gl/GL.h>
#include<math.h>
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 800

boolean gbDone = false;
boolean gbFullScreen = false;
HWND gHwnd;

//variables for fullscreen
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
bool gbEscapePressed = false;
bool gbActiveWindow = false;

HDC ghdc;
HGLRC ghrc;
LRESULT CALLBACK WndProc(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);
	WNDCLASSEX wndclass;
	TCHAR szAppName[] = TEXT("My App");
	HWND hwnd;
	MSG msg;
	RECT rect;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
	RegisterClassEx(&wndclass);
	
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0); //excludes taskbar height also and GetSystemMetrics includes it
	int x = (rect.right-rect.left)/2 - WIN_WIDTH/2;
	int y = ((rect.bottom-rect.top)/2) - WIN_HEIGHT/2;//(GetSystemMetrics(SM_CYSCREEN)/2) - WIN_HEIGHT/2;
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("My OpenGl App"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		x,y, WIN_WIDTH,WIN_HEIGHT,
		//(rect.right-rect.left)/4,(rect.bottom-rect.top)/4, CW_USEDEFAULT,CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	gHwnd = hwnd;
	ShowWindow(hwnd,iCmdShow);
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
			} else {
				gbActiveWindow = false;
			}
		break;
		case WM_SIZE:
		   resize(LOWORD(lParam),HIWORD(lParam));
		break;
		case WM_KEYDOWN:
			switch(wParam){
				case 0x1B: //VK_ESCAPE:
				//gbEscapePressed = true;
				PostQuitMessage(0);
				break;
				case 0x46:
				//MessageBox(hwnd, TEXT("F key is pressed"), TEXT("WM_KEYDOWN event"),MB_OK);
				ToggleFullscreen();
				break;
			}
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullscreen(){
	MONITORINFO mi;
	if(gbFullScreen == false){
		dwStyle = GetWindowLong(gHwnd, GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW){
		mi= {sizeof(MONITORINFO)};
			bool fWndPlacement = GetWindowPlacement(gHwnd, &wpPrev);
			bool fMonitorInfo = GetMonitorInfo(MonitorFromWindow(gHwnd, MONITOR_DEFAULTTOPRIMARY),&mi);
			
			if(fWndPlacement && fMonitorInfo){
				SetWindowLong(gHwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(gHwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom-mi.rcMonitor.top,
				SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}else{
		SetWindowLong(gHwnd, GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(gHwnd, &wpPrev);
		SetWindowPos(gHwnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

void initialize(){
	void resize(int,int);
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
	
	ghdc = GetDC(gHwnd);
	int iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0){
		ReleaseDC(gHwnd,ghdc);
		ghdc = NULL;
	}
	if(SetPixelFormat(ghdc,iPixelFormatIndex,&pfd)==FALSE){
		ReleaseDC(gHwnd,ghdc);
		ghdc = NULL;
	}
	
	ghrc = wglCreateContext(ghdc);
	if(ghrc==NULL){
		ReleaseDC(gHwnd,ghdc);
		ghdc = NULL;
	}
	
	if(wglMakeCurrent(ghdc,ghrc)==FALSE){
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(gHwnd,ghdc);
		ghdc = NULL;
	}
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	//resize(WIN_WIDTH,WIN_HEIGHT); mundi chaat
}

void resize(int width, int height){
	if(height==0)
		height=1;
	
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*glBegin(GL_TRIANGLES);
		glColor3f(1.0f,1.0f,1.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,0.0f);
	glEnd(); */
	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(-1.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
	glEnd();
	
	glLineWidth(1.0f);
	for(float d = -1.0f; d <= 1.0f; d = d + 0.05){	
		glBegin(GL_LINES);
		if(d<0.0f){
			glColor3f(0.0f,0.0f,1.0f);
		}else{
			glColor3f(1.0f,0.0f,0.0f);
		}
			
			glVertex3f(-1.0f,d,0.0f);
			glVertex3f(1.0f,d,0.0f);
		glEnd();
	}
	
	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,-1.0f,0.0f);
	glEnd();
	
	glLineWidth(1.0f);
	for(float d = -1.0f; d <= 1.0f; d = d + 0.05){	
		glBegin(GL_LINES);
			if(d<0.0f){
			glColor3f(0.0f,0.0f,1.0f);
		}else{
			glColor3f(0.0f,1.0f,0.0f);
		}
			glVertex3f(d,1.0f,0.0f);
			glVertex3f(d,-1.0f,0.0f);
		glEnd();
	}
	
	glBegin(GL_LINE_LOOP );
		glColor3f(1.0f,1.0f,0.0f);
		glVertex3f(0.0f,0.5f,0.0f);
		glVertex3f(-0.5f,-0.5f,0.0f);
		glVertex3f(0.5f,-0.5f,0.0f);
	glEnd();
	
	glBegin(GL_LINE_LOOP );
		glColor3f(1.0f,1.0f,0.0f);
		glVertex3f(0.5f,0.5f,0.0f);
		glVertex3f(-0.5f,0.5f,0.0f);
		glVertex3f(-0.5f,-0.5f,0.0f);
		glVertex3f(0.5f,-0.5f,0.0f);
	glEnd();
	
	const float PI = 3.141592f;
	GLint circle_points = 100;
	glBegin(GL_POINTS);
	glColor3f(1.0f,1.0f,0.0f);
	//for(float angle=0.0f;angle<2.0f * PI; angle = angle+ 0.01f){
	for(int i=0;i<circle_points;i++){
		float angle = 2 * PI * i / circle_points;
		glVertex3f(cos(angle)/2, sin(angle)/2, 0.0f);
	}
	glEnd();
	SwapBuffers(ghdc);
}

void uninitialize(void){
	if(gbFullScreen==TRUE){
		SetWindowLong(gHwnd, GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(gHwnd, &wpPrev);
		SetWindowPos(gHwnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
	
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc=NULL;
	ReleaseDC(gHwnd,ghdc);
	ghdc=NULL;
	DestroyWindow(gHwnd);
	gHwnd= NULL;
}