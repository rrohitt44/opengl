//double buffer opengl program
#include<Windows.h>
#include<gl/GL.h>
#include<math.h>
#include<gl/GLU.h>
#define WIN_WIDTH 600
#define WIN_HEIGHT 600

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"glu32.lib")
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
bool gbDone = false;
bool gbFullScreen = false;
HWND ghwnd;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)}; //for fullscreen

GLfloat gfAngleCube = 0.0f;
void updateAngle(){
	gfAngleCube = gfAngleCube + 0.1f;
	if(gfAngleCube>=360.0f){
		gfAngleCube = 0.0f;
	}
}

//changes for opengl
bool gbActiveWindow = false;
bool gbEscapePressed = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLIne, int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);//double buffer sathi
	WNDCLASSEX wndclass;
	TCHAR AppName[] = TEXT("Window Custom");
	TCHAR WinName[] = TEXT("My First OpenGL program - double buffer");
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
				updateAngle();
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
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	//removed as not needed for double buffering
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void display(){
	void drawCircle(void);
	void drawCircleT1(void);
	void drawArc(void);
	void drawLine(void);
	void drawRect(void);
	void drawArc1(void);
	void drawLine1(void);
	void drawArc2(void);
	void doAdjust1(void);
	void doAdjust2(void);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//outer area
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-3.5f);
	
	glRotatef(180.0f, 0.0f,0.0f,1.0f);
	//glColor3f(0.30f,0.30f,1.0f);
	//glColor3f(0.419608f,0.137255f,0.556863f);
	//glColor3f( 0.498039f,0.0f,1.0f);
	glColor3f(0.0f,0.0f,0.35f);
	drawArc();
	drawLine();
	
	//inner area
	GLfloat zTransf = -8.0f+3.5f;
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glTranslatef(-0.75f,-0.75f,zTransf);
	//glRotatef(180.0f, 0.0f,0.0f,1.0f);
	glColor3f(0.0f,0.3f,0.623529f);
	drawArc();
	drawLine();
	
	/*
	
	doAdjust1();
	*/
	
	
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glTranslatef(-0.75f,-0.75f,zTransf);
	//glColor3f(1.0f,1.0f,1.0f);
	//glRotatef(180.0f, 0.0f,0.0f,1.0f);
	glColor3f(0.0f,0.0f,0.35f);
	drawArc1();
	drawLine1();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.75f,-0.75f,zTransf-3.5f);
	//glRotatef(180.0f, 0.0f,0.0f,1.0f);
	glColor3f(0.0f,0.3f,0.623529f);
	drawArc();
	drawLine();
	
	/*
	doAdjust2();
	*/
	/*
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.75f,-0.75f,zTransf);
	glColor3f(0.0f,0.0f,0.35f);
	drawArc1();
	drawLine1();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawRect();
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.8f,0.0f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircle();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.8f,0.0f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircle();
	*/
	SwapBuffers(ghdc);
}

void drawArc2(){
	const float PI = 3.141592f;
	GLint circle_points = 1000;
	glBegin(GL_TRIANGLE_FAN);
	//for(float angle=0.0f;angle<2.0f * PI; angle = angle+ 0.01f){
	for(int i=900;i<circle_points;i++){
		float angle = 2 * PI * i / circle_points;
		glVertex3f(cos(angle)/4, sin(angle)/4, 0.0f);
	}
	glEnd();
	
	
}

void doAdjust1(){
	void drawCircle(void);
	void drawCircleT1(void);
	GLfloat zTransf = -8.0f;
	//kachara start
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.8f,1.5f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircle();
	
	
	
	///
	GLfloat xTrans = 0.7f;
	GLfloat yTrans = 1.5f;
	for(GLint t=1;t<5;t++){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(xTrans,yTrans,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircle();
	xTrans = xTrans-0.05f;
	//yTrans = yTrans-0.01f;
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.34f,1.08f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircleT1();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.27f,0.9f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircleT1();
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.4f,1.20f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircleT1();
	//kachara end
}
void doAdjust2(){
	void drawCircle(void);
	void drawCircleT1(void);
	GLfloat zTransf = -8.0f;
	//kachara start
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.8f,-1.5f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircle();
	
	
	
	///
	
	GLfloat xTrans = 0.7f;
	GLfloat yTrans = 1.5f;
	for(GLint t=1;t<5;t++){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-xTrans,-yTrans,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircle();
	xTrans = xTrans-0.05f;
	//yTrans = yTrans-0.01f;
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.34f,-1.08f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircleT1();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.27f,-0.9f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircleT1();
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.4f,-1.20f,zTransf);
	glColor3f(1.0f,1.0f,1.0f);
	drawCircleT1();
	//kachara end
}
void drawArc(){
	const float PI = 3.141592f;
	GLint circle_points = 1000;
	glBegin(GL_TRIANGLE_FAN);
	//for(float angle=0.0f;angle<2.0f * PI; angle = angle+ 0.01f){
	for(int i=250;i<circle_points;i++){
		float angle = 2 * PI * i / circle_points;
		glVertex3f(cos(angle), sin(angle), 0.0f);
	}
	glEnd();
	
	
}



void drawLine(){
	const float PI = 3.141592f;
	GLint circle_points = 1000;
	glBegin(GL_TRIANGLES);
	int i = 250;
	float ang = 2 * PI * i / circle_points;
	glVertex3f(cos(ang),sin(ang),0.0f);
	
	ang = 2 * PI * i / circle_points;
	glVertex3f(cos(ang)+1.0f,sin(ang),0.0f);
	
	i=999;
	ang = 2 * PI * i / circle_points;
	glVertex3f(cos(ang),sin(ang),0.0f);
	
	glEnd();
}

void drawArc1(){
	const float PI = 3.141592f;
	GLint circle_points = 1000;
	glBegin(GL_TRIANGLE_FAN);
	//for(float angle=0.0f;angle<2.0f * PI; angle = angle+ 0.01f){
	for(int i=250;i<circle_points;i++){
		float angle = 2 * PI * i / circle_points;
		glVertex3f(cos(angle)/2, sin(angle)/2, 0.0f);
	}
	glEnd();
	
	
}

void drawLine1(){
	const float PI = 3.141592f;
	GLint circle_points = 1000;
	glBegin(GL_TRIANGLES);
	int i = 250;
	float ang = 2 * PI * i / circle_points;
	glVertex3f(cos(ang)/2,sin(ang)/2,0.0f);
	
	ang = 2 * PI * i / circle_points;
	glVertex3f((cos(ang)+1.0f)/2,sin(ang)/2,0.0f);
	
	i=999;
	ang = 2 * PI * i / circle_points;
	glVertex3f(cos(ang)/2,sin(ang)/2,0.0f);
	
	glEnd();
}

void drawRect(){
	glBegin(GL_QUADS);
	//vertical
	glVertex3f(-0.24f,0.9f,0.0f);
	glVertex3f(-0.24f,-0.9f,0.0f);
	glVertex3f(0.24f,-0.9f,0.0f);
	glVertex3f(0.24f,0.9f,0.0f);
	
	//horizontal
	glVertex3f(-0.9f,0.24f,0.0f);
	glVertex3f(-0.9f,-0.24f,0.0f);
	glVertex3f(0.9f,-0.24f,0.0f);
	glVertex3f(0.9f,0.24f,0.0f);
	glEnd();
}

void drawCircle(){
	const float PI = 3.141592f;
	GLint circle_points = 1000;
	glBegin(GL_TRIANGLE_FAN);
	//for(float angle=0.0f;angle<2.0f * PI; angle = angle+ 0.01f){
	for(int i=0;i<circle_points;i++){
		float angle = 2 * PI * i / circle_points;
		glVertex3f(cos(angle)/4.1, sin(angle)/4.1, 0.0f);
	}
	glEnd();
}

void drawCircleT1(){
	const float PI = 3.141592f;
	GLint circle_points = 1000;
	glBegin(GL_TRIANGLE_FAN);
	//for(float angle=0.0f;angle<2.0f * PI; angle = angle+ 0.01f){
	for(int i=0;i<circle_points;i++){
		float angle = 2 * PI * i / circle_points;
		glVertex3f(cos(angle)/2, sin(angle)/2, 0.0f);
	}
	glEnd();
}

void resize(int width,int height){
	if(height == 0){
		height = 1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
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