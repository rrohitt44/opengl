//double buffer opengl program - 3D
#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
bool gbDone = false;
bool gbFullScreen = false;
HWND ghwnd;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)}; //for fullscreen

//changes for opengl
bool gbActiveWindow = false;
bool gbEscapePressed = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

//shapes
GLfloat x1Pos = 0.0f;
GLfloat y1Pos = 0.0f;
GLfloat z1Pos = 0.0f;
GLfloat x2Pos = 0.0f;
GLfloat y2Pos = 0.0f;
GLfloat z2Pos = 0.0f;
	
GLUquadric *quadric = NULL;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLIne, int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);//double buffer sathi
	
	WNDCLASSEX wndclass;
	TCHAR AppName[] = TEXT("Window Custom");
	TCHAR WinName[] = TEXT("Primitives");
	HWND hwnd;
	MSG msg;
	RECT rect;
	//initialize window class
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //change for opengl
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = AppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//initialization complete
	
	RegisterClassEx(&wndclass); //Register Class
	
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
	int width = (rect.right-rect.left)/2 - WIN_WIDTH/2;
	int height = (rect.bottom - rect.top)/2 - WIN_HEIGHT/2;
	//int width = GetSystemMetrics(SM_CXSCREEN)/2 - WIN_WIDTH/2;
	//int height = GetSystemMetrics(SM_CYSCREEN)/2 - WIN_HEIGHT/2;
	
	
	//create window in memory
	hwnd = CreateWindowEx(  //change for opengl
	WS_EX_APPWINDOW,
	AppName,
	WinName,
	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, //change for opengl
	width,
	height,
	WIN_WIDTH,
	WIN_HEIGHT,
	NULL,
	NULL,
	hInstance,
	NULL
	);
	ghwnd = hwnd;
	ShowWindow(hwnd, iCmdShow); //Show Window
	//ShowWindow(hwnd, SW_MAXIMIZE); //Show Window
	
	//update window removed, not necessary for opengl
	//UpdateWindow(hwnd); // color/paint the background of the window
	//added below two for opengl
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	
	initialize(); //opengl
	//message loop
	while(gbDone==false){
		
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			if(msg.message == WM_QUIT){
				gbDone = true;
			}else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}else{
			//rendering starts here
			//opengl change
			if(gbActiveWindow == true){
				if(gbEscapePressed == true){
					gbDone = true;
				}
				
				display(); //for double buffer
			}
		}
	}
	
	uninitialize(); //opengl
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
	void ToggleFullscreen(void);
	void resize(int, int);
	//void display(void);
	void uninitialize(void);
	switch(iMsg){
		//case for opengl
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0){
				gbActiveWindow = true;
			}else{
				gbActiveWindow = false;
			}
		break;
		//case added for opengl
		case WM_SIZE:
			resize(LOWORD(lParam),HIWORD(lParam));
		break;
		//WM_PAINT ch painting single buffer ahe. So u will face screen tearing/flickering (state save nhi krta yet - single state)
		/*case WM_PAINT:
			display(); //opengl
		break;*/ //removed as it is not needed for double buffering
		//for opengl
		case WM_CLOSE:
			uninitialize();
		break;
		//for opengl
		/*case WM_ERASEBKGND:
			return (0);
		break;*/ //removed as not needed for double buffering
		case WM_KEYDOWN:
			switch(wParam){
				
				case VK_ESCAPE:
					gbEscapePressed = true;
					break;
				break;
				case 0x46: //F or f key
						ToggleFullscreen();
				break;
			}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullscreen(){
	HMONITOR hMonitor;
	//check kara window fullscreen ahe ka
	if(gbFullScreen == false){
		//get current window style
		 dwStyle = GetWindowLong(ghwnd,GWL_STYLE);
		 
		 //does it includes overlapped window
		if(dwStyle & WS_OVERLAPPEDWINDOW){
			
			//evaluate current window placement
			bool bWindowPlacement = GetWindowPlacement(ghwnd,&wpPrev);
			
			//take the information of the window which you want to make full screen
			hMonitor = MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY);
			MONITORINFO mi;
			mi.cbSize = sizeof(MONITORINFO);
			bool bMonitorInfo = GetMonitorInfo(hMonitor, &mi);
			
			if(bWindowPlacement && bMonitorInfo){
				//set window style accordingly
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				//set window position accordingly
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), 
				(mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			//hide the cursor
			ShowCursor(FALSE);
			gbFullScreen = true;
		}
	}else{
		//code to restore the full screen
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
		//set window placement back to previous
		SetWindowPlacement(ghwnd, &wpPrev);
		//set window position
		SetWindowPos(ghwnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		//show cursor
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

void initialize(){
	void resize(int,int);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	//initialization of pfd
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; //for double buffer
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32; //added to make program 3d
	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0){
		ReleaseDC(ghwnd, ghdc);
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
	
	if(wglMakeCurrent(ghdc,ghrc)== FALSE){
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	
	//added for 3D support
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	//added for 3D support but are optional
	//glShadeModel(GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//removed as not needed for double buffering
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void display(){
	void drawPointGridV1(void);
	void drawLineGridV1(void);
	void drawLineGridV2(void);
	void drawLineGridV3(void);
	void drawLineGridV4(void);
	void drawLineGridV5(void);
	void drawPolygon1(void);
	void drawPolygon2(void);
	void drawPolygon3(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //GL_DEPTH_BUFFER_BIT added for 3D support
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.0f,0.3f,-3.0f);
	drawPointGridV1();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.3f,-3.0f);
	drawLineGridV1();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.0f,0.3f,-3.0f);
	drawLineGridV2();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.0f,-0.3f,-3.0f);
	drawLineGridV3();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,-0.3f,-3.0f);
	drawLineGridV4();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.0f,-0.3f,-3.0f);
	drawPolygon1();
	drawPolygon2();
	drawPolygon3();
	drawLineGridV5();
	SwapBuffers(ghdc);
}

void drawTriangle(){
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,0.0f);
	glEnd();
}

void drawLineGridV5(){
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	//horizontal
		x1Pos=0.0f;
		y1Pos=0.1f;
		x2Pos=0.3f;
		y2Pos=0.1f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
		x1Pos=0.0f;
		y1Pos=y1Pos+0.1f;
		x2Pos=0.3f;
		y2Pos=y2Pos+0.1f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
		
	//verticals
		x1Pos=0.1f;
		y1Pos=0.0f;
		x2Pos=0.1f;
		y2Pos=0.3f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
		x1Pos=x1Pos+0.1f;
		y1Pos=0.0f;
		x2Pos=x2Pos+0.1f;
		y2Pos=0.3f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
	glEnd();
}
void drawLineGridV4(){
		void drawSquare(void);
		void drawDiagonalsFromSamePoint(void);
		drawSquare();
		drawDiagonalsFromSamePoint();
}

void drawPolygon1(){
	glBegin(GL_POLYGON);
	glColor3f(1.0f,0.0f,0.0f);
	x1Pos=0.0f;
	y1Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.0f;
	y1Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.1f;
	y1Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.1f;
	y1Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glEnd();
}
void drawPolygon2(){
	glBegin(GL_POLYGON);
	glColor3f(0.0f,1.0f,0.0f);
	x1Pos=0.1f;
	y1Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.2f;
	y1Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.2f;
	y1Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.1f;
	y1Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glEnd();
}
void drawPolygon3(){
	glBegin(GL_POLYGON);
	glColor3f(0.0f,0.0f,!.0f);
	x1Pos=0.2f;
	y1Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.3f;
	y1Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.3f;
	y1Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.2f;
	y1Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glEnd();
}
void drawSquare(){
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f,1.0f,1.0f);
	x1Pos=0.0f;
	y1Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	y1Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	x1Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	y1Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glEnd();
}

void drawDiagonalsFromSamePoint(){
		glBegin(GL_LINES);
		glColor3f(1.0f,1.0f,1.0f);
		//diagonal
		x1Pos=0.0f;
		y1Pos=0.3f;
		x2Pos=0.3f;
		y2Pos=0.2f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
		
		//x1Pos=0.0f;
		//y1Pos=0.3f;
		x2Pos=0.3f;
		y2Pos=y2Pos-0.1f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
		
		//x1Pos=0.0f;
		//y1Pos=0.3f;
		x2Pos=0.3f;
		y2Pos=y2Pos-0.1f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
		
		//x1Pos=0.0f;
		//y1Pos=0.3f;
		x2Pos=x2Pos-0.1f;
		//y2Pos=0.0f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
		
		//x1Pos=0.0f;
		//y1Pos=0.3f;
		x2Pos=x2Pos-0.1f;
		//y2Pos=0.0f;
		glVertex3f(x1Pos,y1Pos,z1Pos);
		glVertex3f(x2Pos,y2Pos,z2Pos);
		glEnd();
}

void drawLineGridV3(){
	void drawLineGridV2(void);
	void drawDiagonals(void);
	drawLineGridV2();
	drawDiagonals();	
}

void drawDiagonals(){
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	//diagonal
	x1Pos=0.0f;
	y1Pos=0.2f;
	x2Pos=0.1f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	//x1Pos
	y1Pos=y1Pos-0.1f;
	x2Pos=x2Pos+0.1f;
	//y2Pos
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	//x1Pos
	y1Pos=y1Pos-0.1f;
	x2Pos=x2Pos+0.1f;
	//y2Pos
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	//y1Pos=0.0f;
	//x2Pos
	y2Pos=y2Pos-0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	//y1Pos=0.0f;
	//x2Pos
	y2Pos=y2Pos-0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	//diagonal end
	glEnd();
}

void drawVerticalLines(){
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	//vertical
	x1Pos=0.0f;
	y1Pos=0.0f;
	x2Pos=0.0f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	y1Pos=0.0f;
	x2Pos=x2Pos+0.1f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	y1Pos=0.0f;
	x2Pos=x2Pos+0.1f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	y1Pos=0.0f;
	x2Pos=x2Pos+0.1f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	//vertical end
	glEnd();
}

void drawHorizontal(){
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	//horizontal
	x1Pos=0.0f;
	y1Pos=0.0f;
	x2Pos=0.3f;
	y2Pos=0.0f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=0.0f;
	y1Pos=y1Pos+0.1f;
	x2Pos=0.3f;
	y2Pos=y2Pos+0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=0.0f;
	y1Pos=y1Pos+0.1f;
	x2Pos=0.3f;
	y2Pos=y2Pos+0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=0.0f;
	y1Pos=y1Pos+0.1f;
	x2Pos=0.3f;
	y2Pos=y2Pos+0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	glEnd();
}

void drawLineGridV2(){
	void drawHorizontal(void);
	void drawVerticalLines(void);
	drawHorizontal();
	drawVerticalLines();
}

void drawLineGridV1(){
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	
	//vertical
	x1Pos=0.0f;
	y1Pos=0.0f;
	x2Pos=0.0f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	y1Pos=0.0f;
	x2Pos=x2Pos+0.1f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	y1Pos=0.0f;
	x2Pos=x2Pos+0.1f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	//vertical end
	
	//horizontal
	x1Pos=0.0f;
	y1Pos=0.1f;
	x2Pos=0.3f;
	y2Pos=0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	//x1Pos won't change here
	y1Pos=y1Pos+0.1f;
	//x2Pos;
	y2Pos=y2Pos+0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	//x1Pos
	y1Pos=y1Pos+0.1f;
	//x2Pos
	y2Pos=y2Pos+0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	//horizontal end
	
	//diagonal
	x1Pos=0.0f;
	y1Pos=0.2f;
	x2Pos=0.1f;
	y2Pos=0.3f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	//x1Pos
	y1Pos=y1Pos-0.1f;
	x2Pos=x2Pos+0.1f;
	//y2Pos
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	//x1Pos
	y1Pos=y1Pos-0.1f;
	x2Pos=x2Pos+0.1f;
	//y2Pos
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	//y1Pos=0.0f;
	//x2Pos
	y2Pos=y2Pos-0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	
	x1Pos=x1Pos+0.1f;
	//y1Pos=0.0f;
	//x2Pos
	y2Pos=y2Pos-0.1f;
	glVertex3f(x1Pos,y1Pos,z1Pos);
	glVertex3f(x2Pos,y2Pos,z2Pos);
	//diagonal end
	glEnd();
}

void drawPointGridV1(){
	glBegin(GL_POINTS);
	glColor3f(1.0f,1.0f,1.0f);
	for(float i=0.0f;i<0.4f;i=i+0.1f){
		for(float j=0.0f;j<0.4f;j=j+0.1f){
			glVertex3f(i,j,0.0f);
		}
	}
	glEnd();
}

void drawPointGridV2(){
	GLfloat xPos = 0.0f;
	GLfloat yPos = 0.0f;
	GLfloat zPos = 0.0f;
	glBegin(GL_POINTS);
	glColor3f(1.0f,1.0f,1.0f);
	
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	
	xPos = 0.0f;
	yPos = yPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	
	
	xPos = 0.0f;
	yPos = yPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	
	xPos = 0.0f;
	yPos = yPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	xPos = xPos + 0.1f;
	glVertex3f(xPos,yPos,zPos);
	glEnd();
}
void resize(int width,int height){
	if(height == 0){
		height = 1;
	}
	
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat) width/ (GLfloat) height, 0.1f,100.0f);
}

void uninitialize(){
	if(gbFullScreen == true){
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
	
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc = NULL;
	
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;
	
	DestroyWindow(ghwnd);
	ghwnd = NULL;
}