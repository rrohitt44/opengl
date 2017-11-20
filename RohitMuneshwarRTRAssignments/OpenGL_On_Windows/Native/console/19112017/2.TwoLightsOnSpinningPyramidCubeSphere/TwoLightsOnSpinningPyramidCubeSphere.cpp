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

GLfloat gfAnglePira = 0.0f;
GLfloat gfAngleCube = 0.0f;
GLfloat gfAngleSphere = 0.0f;
GLUquadric *quadric = NULL;

//light0
GLfloat light0_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat light0_defused[]={1.0f,0.0f,0.0f,0.0f};
GLfloat light0_specular[]={1.0f,0.0f,0.0f,0.0f};
GLfloat light0_position[]={2.0f,1.0f,1.0f,0.0f};

//light1
GLfloat light1_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat light1_defused[]={0.0f,0.0f,1.0f,0.0f};
GLfloat light1_specular[]={0.0f,0.0f,1.0f,0.0f};
GLfloat light1_position[]={-2.0f,1.0f,1.0f,0.0f};

//material
GLfloat material_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat material_defused[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_shininess[]={50.0f};


bool gbLightMode = false;
GLboolean gbPyramidMode = GL_TRUE;
GLboolean gbCubeMode = GL_FALSE;
GLboolean gbSphereMode = GL_FALSE;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLIne, int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);//double buffer sathi
	void updateAngle(void);
	WNDCLASSEX wndclass;
	TCHAR AppName[] = TEXT("Window Custom");
	TCHAR WinName[] = TEXT("Lights - Two Lights On Spinning Pyramid,Cube,Sphere");
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
	gfAnglePira = gfAnglePira + 0.01f;
	if(gfAnglePira>=360.0f){
		gfAnglePira = 0.0f;
	}
	
	gfAngleCube = gfAngleCube + 0.01f;
	if(gfAngleCube>=360.0f){
		gfAngleCube = 0.0f;
	}
	
	gfAngleSphere = gfAngleSphere + 0.01f;
	if(gfAngleSphere>=360.0f){
		gfAngleSphere = 0.0f;
	}
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
	void ToggleFullscreen(void);
	void toggleLightMode(void);
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
				case 0x4C: //L
					toggleLightMode();
				break;
				case 0x43: //C
					gbCubeMode = GL_TRUE;
					gbPyramidMode = GL_FALSE;
					gbSphereMode = GL_FALSE;
				break;
				case 0x50: //P
					gbCubeMode = GL_FALSE;
					gbPyramidMode = GL_TRUE;
					gbSphereMode = GL_FALSE;
				break;
				case 0x53: //S
					gbCubeMode = GL_FALSE;
					gbPyramidMode = GL_FALSE;
					gbSphereMode = GL_TRUE;
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
	
	glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_defused);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
	glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
	
	glLightfv(GL_LIGHT1,GL_AMBIENT,light1_ambient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,light1_defused);
	glLightfv(GL_LIGHT1,GL_SPECULAR,light1_specular);
	glLightfv(GL_LIGHT1,GL_POSITION,light1_position);
	
	glMaterialfv(GL_FRONT,GL_AMBIENT,material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_defused);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void display(){
	void DrawPyramid(void);
	void DrawCube(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //GL_DEPTH_BUFFER_BIT added for 3D support
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if(gbPyramidMode==GL_TRUE){
		glTranslatef(-1.5f,0.0f,-6.0f);
		glRotatef(gfAnglePira, 0.0f,1.0f,0.0f);
		DrawPyramid();
	}else if(gbCubeMode==GL_TRUE){
		glTranslatef(1.5f,0.0f,-6.0f);
		glScalef(0.75f,0.75f,0.75f);
		glRotatef(gfAngleCube, 0.0f,1.0f,0.0f);
		DrawCube();
	}else if(gbSphereMode==GL_TRUE){
		glTranslatef(0.0f,0.0f,-3.0f);
		glRotatef(gfAngleSphere,1.0f,0.0,0.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		quadric = gluNewQuadric();
		glColor3f(1.0f,1.0f,0.0f);
		gluSphere(quadric, 0.75f,30,30);
	}
	SwapBuffers(ghdc);
}

void DrawPyramid(){
	glBegin(GL_TRIANGLES);
	//front
	glNormal3f(0.0f,0.447214f,0.894427f);
	glColor3f(1.0f,0.0f,0.0f); //red
	glVertex3f(0.0f,1.0f,0.0f); //appex
	glColor3f(0.0f,1.0f,0.0f); //green
	glVertex3f(-1.0f,-1.0f,1.0f);
	glColor3f(0.0f,0.0f,1.0f); //blue
	glVertex3f(1.0f,-1.0f,1.0f);
	
	//right
	glNormal3f(0.894427f,0.447214f,0.0f);
	glColor3f(1.0f,0.0f,0.0f); //red
	glVertex3f(0.0f,1.0f,0.0f); //appex
	glColor3f(0.0f,0.0f,1.0f); //blue
	glVertex3f(1.0f,-1.0f,1.0f);
	glColor3f(0.0f,1.0f,0.0f); //green
	glVertex3f(1.0f,-1.0f,-1.0f);
	
	//back
	glNormal3f(0.0f,0.447214f,-0.0894427f);
	glColor3f(1.0f,0.0f,0.0f); //red
	glVertex3f(0.0f,1.0f,0.0f); //appex
	glColor3f(0.0f,1.0f,0.0f); //green
	glVertex3f(1.0f,-1.0f,-1.0f);
	glColor3f(0.0f,0.0f,1.0f); //blue
	glVertex3f(-1.0f,-1.0f,-1.0f);
	
	//left
	glNormal3f(-0.894427f,0.447214f,0.0f);
	glColor3f(1.0f,0.0f,0.0f); //red
	glVertex3f(0.0f,1.0f,0.0f); //appex
	glColor3f(0.0f,0.0f,1.0f); //blue
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glColor3f(0.0f,1.0f,0.0f); //green
	glVertex3f(-1.0f,-1.0f,1.0f);
	glEnd();
}

void DrawCube(){
	glBegin(GL_QUADS);
	
	//front
	glNormal3f(0.0f,0.0f,1.0f);
	glColor3f(0.0f,0.0f,1.0f); //blue
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	
	//right
	glNormal3f(1.0f,0.0f,0.0f);
	glColor3f(1.0f,0.0f,1.0f); //magenta
	glVertex3f(1.0f,1.0f,-1.0f);
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	
	//back
	glNormal3f(0.0f,0.0f,-1.0f);
	glColor3f(0.0f,1.0f,1.0f); //cyan
	glVertex3f(1.0f,1.0f,-1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	
	//left
	glNormal3f(-1.0f,0.0f,0.0f);
	glColor3f(1.0f,1.0f,0.0f); //yellow
	glVertex3f(-1.0f,1.0f,-1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	
	//top
	glNormal3f(0.0f,1.0f,0.0f);
	glColor3f(1.0f,0.0f,0.0f); //red
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);
	glVertex3f(1.0f,1.0f,-1.0f);
	
	//bottom
	glNormal3f(0.0f,-1.0f,0.0f);
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
	
	if(quadric!=NULL){
		quadric=NULL;
	}
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc = NULL;
	
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;
	
	DestroyWindow(ghwnd);
	ghwnd = NULL;
}

void toggleLightMode()
{
	if(gbLightMode==false)
	{
		glEnable(GL_LIGHTING);
		gbLightMode=true;
	}else
	{
		glDisable(GL_LIGHTING);
		gbLightMode=false;
	}
}
