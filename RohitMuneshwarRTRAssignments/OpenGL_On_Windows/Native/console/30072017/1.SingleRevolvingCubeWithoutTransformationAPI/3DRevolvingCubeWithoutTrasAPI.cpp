//double buffer opengl program - 3D
#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
//#define USE_MATH_DEFINES 1
#include<math.h>
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define M_PI 3.1415
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

GLfloat gfAngle_degree = 0.0f;
GLfloat gfAngle_radian = 0.0f;
//needed for transformation without using transformation APIs
GLfloat identityMatrix[16];
GLfloat translationMatrix[16];
GLfloat scaleMatrix[16];
GLfloat x_rotationMatrix[16];
GLfloat y_rotationMatrix[16];
GLfloat z_rotationMatrix[16];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLIne, int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);//double buffer sathi
	void updateAngle(void);
	WNDCLASSEX wndclass;
	TCHAR AppName[] = TEXT("Window Custom");
	TCHAR WinName[] = TEXT("3D Pyramid");
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

void updateAngle(){
	gfAngle_degree = gfAngle_degree + 0.1f;
	if(gfAngle_degree>=360.0f){
		gfAngle_degree = 0.0f;
	}
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
	//initialization of identityMatrix
	identityMatrix[0]=1.0f;
	identityMatrix[1]=0.0f;
	identityMatrix[2]=0.0f;
	identityMatrix[3]=0.0f;
	identityMatrix[4]=0.0f;
	identityMatrix[5]=1.0f;
	identityMatrix[6]=0.0f;
	identityMatrix[7]=0.0f;
	identityMatrix[8]=0.0f;
	identityMatrix[9]=0.0f;
	identityMatrix[10]=1.0f;
	identityMatrix[11]=0.0f;
	identityMatrix[12]=0.0f;
	identityMatrix[13]=0.0f;
	identityMatrix[14]=0.0f;
	identityMatrix[15]=1.0f;
	//initialization of translationMatrix
	translationMatrix[0]=1.0f;
	translationMatrix[1]=0.0f;
	translationMatrix[2]=0.0f;
	translationMatrix[3]=0.0f;
	translationMatrix[4]=0.0f;
	translationMatrix[5]=1.0f;
	translationMatrix[6]=0.0f;
	translationMatrix[7]=0.0f;
	translationMatrix[8]=0.0f;
	translationMatrix[9]=0.0f;
	translationMatrix[10]=1.0f;
	translationMatrix[11]=0.0f;
	translationMatrix[12]=0.0f;//tx
	translationMatrix[13]=0.0f;//ty
	translationMatrix[14]=-6.0f;//tz
	translationMatrix[15]=1.0f;
	//initialization of scaleMatrix
	scaleMatrix[0]=0.75f;//sx
	scaleMatrix[1]=0.0f;
	scaleMatrix[2]=0.0f;
	scaleMatrix[3]=0.0f;
	scaleMatrix[4]=0.0f;
	scaleMatrix[5]=0.75f;//sy
	scaleMatrix[6]=0.0f;
	scaleMatrix[7]=0.0f;
	scaleMatrix[8]=0.0f;
	scaleMatrix[9]=0.0f;
	scaleMatrix[10]=0.75f;//sz
	scaleMatrix[11]=0.0f;
	scaleMatrix[12]=0.0f;
	scaleMatrix[13]=0.0f;
	scaleMatrix[14]=0.0f;
	scaleMatrix[15]=1.0f;
	//added for 3D support but are optional
	//glShadeModel(GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//removed as not needed for double buffering
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void display(){
	void DrawCube(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //GL_DEPTH_BUFFER_BIT added for 3D support
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glLoadMatrixf(identityMatrix);
	//glTranslatef(0.0f,0.0f,-6.0f);
	glMultMatrixf(translationMatrix);
	//glScalef(0.75f,0.75f,0.75f);
	glMultMatrixf(scaleMatrix);
	//glRotatef(gfAngleCube, 1.0f,0.0f,0.0f);
	//glRotatef(gfAngleCube, 0.0f,1.0f,0.0f);
	//glRotatef(gfAngleCube, 0.0f,0.0f,1.0f);
	
	//convert to radian
	gfAngle_radian = gfAngle_degree * (M_PI/180.0f);
	//initialization of x_rotationMatrix
	x_rotationMatrix[0]=1.0f;
	x_rotationMatrix[1]=0.0f;
	x_rotationMatrix[2]=0.0f;
	x_rotationMatrix[3]=0.0f;
	x_rotationMatrix[4]=0.0f;
	x_rotationMatrix[5]=cos(gfAngle_radian);
	x_rotationMatrix[6]=sin(gfAngle_radian);
	x_rotationMatrix[7]=0.0f;
	x_rotationMatrix[8]=0.0f;
	x_rotationMatrix[9]=-sin(gfAngle_radian);
	x_rotationMatrix[10]=cos(gfAngle_radian);
	x_rotationMatrix[11]=0.0f;
	x_rotationMatrix[12]=0.0f;
	x_rotationMatrix[13]=0.0f;
	x_rotationMatrix[14]=0.0f;
	x_rotationMatrix[15]=1.0f;
	glMultMatrixf(x_rotationMatrix);
	//initialization of y_rotationMatrix
	y_rotationMatrix[0]=cos(gfAngle_radian);
	y_rotationMatrix[1]=0.0f;
	y_rotationMatrix[2]=-sin(gfAngle_radian);
	y_rotationMatrix[3]=0.0f;
	y_rotationMatrix[4]=0.0f;
	y_rotationMatrix[5]=1.0f;
	y_rotationMatrix[6]=0.0f;
	y_rotationMatrix[7]=0.0f;
	y_rotationMatrix[8]=sin(gfAngle_radian);
	y_rotationMatrix[9]=0.0f;
	y_rotationMatrix[10]=cos(gfAngle_radian);
	y_rotationMatrix[11]=0.0f;
	y_rotationMatrix[12]=0.0f;
	y_rotationMatrix[13]=0.0f;
	y_rotationMatrix[14]=0.0f;
	y_rotationMatrix[15]=1.0f;
	glMultMatrixf(y_rotationMatrix);
	//initialization of z_rotationMatrix
	z_rotationMatrix[0]=cos(gfAngle_radian);
	z_rotationMatrix[1]=sin(gfAngle_radian);
	z_rotationMatrix[2]=0.0f;
	z_rotationMatrix[3]=0.0f;
	z_rotationMatrix[4]=-sin(gfAngle_radian);
	z_rotationMatrix[5]=cos(gfAngle_radian);
	z_rotationMatrix[6]=0.0f;
	z_rotationMatrix[7]=0.0f;
	z_rotationMatrix[8]=0.0f;
	z_rotationMatrix[9]=0.0f;
	z_rotationMatrix[10]=1.0f;
	z_rotationMatrix[11]=0.0f;
	z_rotationMatrix[12]=0.0f;
	z_rotationMatrix[13]=0.0f;
	z_rotationMatrix[14]=0.0f;
	z_rotationMatrix[15]=1.0f;
	glMultMatrixf(z_rotationMatrix);
	DrawCube();
	SwapBuffers(ghdc);
}

void DrawCube(){
	glBegin(GL_QUADS);
	
	//front
	glColor3f(0.0f,0.0f,1.0f); //blue
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	
	//right
	glColor3f(1.0f,0.0f,1.0f); //magenta
	glVertex3f(1.0f,1.0f,-1.0f);
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	
	//back
	glColor3f(0.0f,1.0f,1.0f); //cyan
	glVertex3f(1.0f,1.0f,-1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	
	//left
	glColor3f(1.0f,1.0f,0.0f); //yellow
	glVertex3f(-1.0f,1.0f,-1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	
	//top
	glColor3f(1.0f,0.0f,0.0f); //red
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);
	glVertex3f(1.0f,1.0f,-1.0f);
	
	//bottom
	glColor3f(0.0f,1.0f,0.0f); //green
	glVertex3f(1.0f,-1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
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