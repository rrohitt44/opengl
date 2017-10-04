//double buffer opengl program - 3D
#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>
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

GLUquadric *quadric = NULL;
//circle
GLfloat PI = 3.1415;
GLfloat CIRCLE_SIZE = 0.04f;
GLfloat POLE_HEIGHT = 0.5f;
GLfloat CABINATE_HEIGHT = 0.3f;
GLfloat TOTAL_HEIGHT = POLE_HEIGHT+CABINATE_HEIGHT;
GLfloat CABINATE_WIDTH = 0.2f;
GLfloat XLOC = 0.08f;
bool gbRPressed = false;
bool gbGPressed = true;
bool gbYPressed = false;
GLfloat zTrans = -3.0f;
GLfloat yTrans = POLE_HEIGHT;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLIne, int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);//double buffer sathi
	
	WNDCLASSEX wndclass;
	TCHAR AppName[] = TEXT("Window Custom");
	TCHAR WinName[] = TEXT("Trafic Signal");
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
				case 0x31:
					gbRPressed = true;
					gbGPressed = true;
					gbYPressed = true;
				break;
				case 0x32:
					gbRPressed = false;
					gbGPressed = false;
					gbYPressed = false;
				break;
				case 'R':
					if(gbRPressed==false)
						gbRPressed = true;
					else
						gbRPressed = false;
				break;
				case 'G':
					if(gbGPressed==false)
						gbGPressed = true;
					else
						gbGPressed = false;
				break;
				case 'Y':
					if(gbYPressed==false)
						gbYPressed = true;
					else
						gbYPressed = false;
				break;
				case VK_ESCAPE:
					gbEscapePressed = true;
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
	void drawTrafficSignal(void);
	void drawAxis(void);
	void drawGround(void);
	void drawBuildingInFirstQuadrant(void);
	void drawBuildingInSecondQuadrant(void);
	void drawCar(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //GL_DEPTH_BUFFER_BIT added for 3D support
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-3.0f);
	
	
	drawAxis();
	drawGround();
	drawTrafficSignal();
	
	drawBuildingInFirstQuadrant();
	drawBuildingInSecondQuadrant();
	
	//glRotatef(90,1.0f,1.0f,0.0f);
	drawCar();
	SwapBuffers(ghdc);
}

void drawTrafficSignal(){
	void drawRCircle(GLfloat,GLfloat,GLfloat);
	void drawGCircle(GLfloat,GLfloat,GLfloat);
	void drawYCircle(GLfloat,GLfloat,GLfloat);
	void drawCabinate(void);
	void drawPipe(void);
	
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zTrans);
	drawCabinate();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,yTrans+0.25f,zTrans);
	drawRCircle(0.0f,0.0f,0.0f);
	if(gbRPressed==true){
		drawRCircle(1.0f,0.0f,0.0f);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,yTrans+0.15f,zTrans);
	drawYCircle(0.0f,0.0f,0.0f);
	if(gbYPressed==true){
		drawYCircle(1.0f,1.0f,0.0f);
	}
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.05f+yTrans,zTrans);
	drawGCircle(0.0f,0.0f,0.0f);
	if(gbGPressed==true){
		drawGCircle(0.0f,1.0f,0.0f);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zTrans);
	drawPipe();
}

void drawCabinate(){
	void drawQuad(void);
	void drawQuad1(void);
	void drawQuad2(void);
	void drawQuad3(void);
	void drawQuad4(void);
	void drawQuad5(void);
	void drawQuad6(void);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-3.0f);
	drawQuad();
	drawQuad1();
	drawQuad2();
	drawQuad3();
	drawQuad4();
	drawQuad5();
	drawQuad6();
}
void drawQuad(){
	glBegin(GL_QUADS);
	glColor3f(0.40f,0.40f,0.40f);
	glVertex3f(XLOC,POLE_HEIGHT,0.0f);
	glVertex3f(-XLOC,POLE_HEIGHT,0.0f);
	glVertex3f(-XLOC,POLE_HEIGHT+CABINATE_HEIGHT,0.0f);
	glVertex3f(XLOC,POLE_HEIGHT+CABINATE_HEIGHT,0.0f);
	glEnd();
}

void drawQuad1(){
	GLfloat sideMargin = 0.01f;
	GLfloat lPg = 0.07f;;
	glBegin(GL_QUADS);
	glColor3f(0.40f,0.40f,0.40f);
	glVertex3f(XLOC,TOTAL_HEIGHT-sideMargin,0.0f);
	glVertex3f(XLOC,TOTAL_HEIGHT-lPg - sideMargin,0.0f);
	glVertex3f((GLfloat)(((CABINATE_WIDTH-lPg - sideMargin)*3/4)/2)+XLOC,TOTAL_HEIGHT-lPg - sideMargin,0.0f);
	glVertex3f(XLOC+(CABINATE_WIDTH-lPg - sideMargin)*3/4,TOTAL_HEIGHT - sideMargin,0.0f);
	glEnd();
}

void drawQuad2(){
	GLfloat sideMargin = 0.01f;
	GLfloat lPg = 0.07f;
	GLfloat gap = 0.035f;
	glBegin(GL_QUADS);
	glColor3f(0.40f,0.40f,0.40f);
	glVertex3f(XLOC,TOTAL_HEIGHT-sideMargin-lPg-gap,0.0f);
	glVertex3f(XLOC,TOTAL_HEIGHT-lPg-lPg-sideMargin-gap,0.0f);
	glVertex3f((GLfloat)(((CABINATE_WIDTH-lPg - sideMargin)*3/4)/2)+XLOC,TOTAL_HEIGHT-lPg-lPg-sideMargin-gap,0.0f);
	glVertex3f(XLOC+(CABINATE_WIDTH-lPg - sideMargin)*3/4,TOTAL_HEIGHT-sideMargin-lPg-gap,0.0f);
	glEnd();
}

void drawQuad3(){
	GLfloat sideMargin = 0.01f;
	GLfloat lPg = 0.07f;
	GLfloat gap = 0.035f;
	glBegin(GL_QUADS);
	glColor3f(0.40f,0.40f,0.40f);
	glVertex3f(XLOC,TOTAL_HEIGHT-sideMargin-lPg-gap-lPg-gap,0.0f);
	glVertex3f(XLOC,TOTAL_HEIGHT-lPg-lPg-lPg-sideMargin-gap-gap,0.0f);
	glVertex3f((GLfloat)(((CABINATE_WIDTH-lPg - sideMargin)*3/4)/2)+XLOC,TOTAL_HEIGHT-lPg-lPg-lPg-sideMargin-gap-gap,0.0f);
	glVertex3f(XLOC+(CABINATE_WIDTH-lPg - sideMargin)*3/4,TOTAL_HEIGHT-sideMargin-lPg-gap-lPg-gap,0.0f);
	
	
	glEnd();
}

void drawQuad4(){
	GLfloat sideMargin = 0.01f;
	GLfloat lPg = 0.07f;
	GLfloat gap = 0.035f;
	glBegin(GL_QUADS);
	glColor3f(0.40f,0.40f,0.40f);
	glVertex3f(-XLOC,TOTAL_HEIGHT-sideMargin,0.0f);
	glVertex3f(-XLOC,TOTAL_HEIGHT-lPg - sideMargin,0.0f);
	glVertex3f(-((GLfloat)(((CABINATE_WIDTH-lPg - sideMargin)*3/4)/2)+XLOC),TOTAL_HEIGHT-lPg - sideMargin,0.0f);
	glVertex3f(-(XLOC+(CABINATE_WIDTH-lPg - sideMargin)*3/4),TOTAL_HEIGHT - sideMargin,0.0f);
	
	
	glEnd();
}

void drawQuad5(){
	GLfloat sideMargin = 0.01f;
	GLfloat lPg = 0.07f;
	GLfloat gap = 0.035f;
	glBegin(GL_QUADS);
	glColor3f(0.40f,0.40f,0.40f);
	glVertex3f(-XLOC,TOTAL_HEIGHT-sideMargin-lPg-gap,0.0f);
	glVertex3f(-XLOC,TOTAL_HEIGHT-lPg-lPg-sideMargin-gap,0.0f);
	glVertex3f(-((GLfloat)(((CABINATE_WIDTH-lPg - sideMargin)*3/4)/2)+XLOC),TOTAL_HEIGHT-lPg-lPg-sideMargin-gap,0.0f);
	glVertex3f(-(XLOC+(CABINATE_WIDTH-lPg - sideMargin)*3/4),TOTAL_HEIGHT-sideMargin-lPg-gap,0.0f);
	
	
	glEnd();
}

void drawQuad6(){
	GLfloat sideMargin = 0.01f;
	GLfloat lPg = 0.07f;
	GLfloat gap = 0.035f;
	glBegin(GL_QUADS);
	glColor3f(0.40f,0.40f,0.40f);
	glVertex3f(-XLOC,TOTAL_HEIGHT-sideMargin-lPg-gap-lPg-gap,0.0f);
	glVertex3f(-XLOC,TOTAL_HEIGHT-lPg-lPg-lPg-sideMargin-gap-gap,0.0f);
	glVertex3f(-((GLfloat)(((CABINATE_WIDTH-lPg - sideMargin)*3/4)/2)+XLOC),TOTAL_HEIGHT-lPg-lPg-lPg-sideMargin-gap-gap,0.0f);
	glVertex3f(-(XLOC+(CABINATE_WIDTH-lPg - sideMargin)*3/4),TOTAL_HEIGHT-sideMargin-lPg-gap-lPg-gap,0.0f);
	
	
	glEnd();
}



void drawGround(){
	void drawHorizontalRoad(void);
	void drawVerticalRoad(void);
	void drawSecondQuadrantGreenArea(void);
	void drawThirdQuadrantGreenArea(void);
	void drawFirstQuadrantGreenArea(void);
	void drawFourthQuadrantGreenArea(void);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-3.0f);
	
	drawHorizontalRoad();
	drawVerticalRoad();
	drawFirstQuadrantGreenArea();
	drawFourthQuadrantGreenArea();
	drawThirdQuadrantGreenArea();
	drawSecondQuadrantGreenArea();
}
void drawPipe(){
	glBegin(GL_QUADS);
	glColor3f(0.40f,0.40f,0.40f);
	glVertex3f(-0.03f,POLE_HEIGHT,0.0f);
	glVertex3f(0.03f,POLE_HEIGHT,0.0f);
	glVertex3f(0.03f,0.0f,0.0f);
	glVertex3f(-0.03f,0.0f,0.0f);
	glEnd();
	
	glBegin(GL_TRIANGLES);
	glColor3f(0.40f,0.40f,0.40f);
	//front
	glVertex3f(0.0f,0.2f,-0.25f);
	glVertex3f(0.05f,0.0f,0.0f);
	glVertex3f(-0.05f,0.0f,0.0f);
	
	//right
	glVertex3f(0.0f,0.2f,-0.25f);
	glVertex3f(0.05f,0.0f,0.0f);
	glVertex3f(0.05f,0.0f,-0.05f);
	
	//back
	glVertex3f(0.0f,0.2f,-0.25f);
	glVertex3f(0.05f,0.0f,-0.05f);
	glVertex3f(-0.05f,0.0f,-0.05f);
	
	//left
	glVertex3f(0.0f,0.2f,-0.25f);
	glVertex3f(-0.05f,0.0f,0.0f);
	glVertex3f(-0.05f,0.0f,-0.05f);
	glEnd();
}
void drawSphere(){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	glColor3f(1.0f,1.0f,0.0f);
	glScalef(0.0f,0.0f,0.0f);
	gluSphere(quadric, 0.75f,30,30);
}
void drawRCircle(GLfloat lRColor,GLfloat lGColor,GLfloat lBColor){
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(lRColor,lGColor,lBColor);
	for(float angle=0.0f;angle<(2.0f*PI); angle = angle + 0.01){
		glVertex3f(cos(angle)*CIRCLE_SIZE,sin(angle)*CIRCLE_SIZE,0.0f);
	}
	glEnd();
}


void drawGCircle(GLfloat lRColor,GLfloat lGColor,GLfloat lBColor){
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(lRColor,lGColor,lBColor);
	for(float angle=0.0f;angle<(2.0f*PI); angle = angle + 0.0001){
		glVertex3f(cos(angle)*CIRCLE_SIZE,sin(angle)*CIRCLE_SIZE,0.0f);
	}
	glEnd();
}

void drawYCircle(GLfloat lRColor,GLfloat lGColor,GLfloat lBColor){
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(lRColor,lGColor,lBColor);
	for(float angle=0.0f;angle<(2.0f*PI); angle = angle + 0.0001){
		glVertex3f(cos(angle)*CIRCLE_SIZE,sin(angle)*CIRCLE_SIZE,0.0f);
	}
	glEnd();
}

void drawAxis(){
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,-1.0f,0.0f);
	glVertex3f(-1.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glEnd();
}
void drawVerticalRoad(){
	GLfloat lzTrans = 1.0f;
	GLfloat lyTrans = 0.1f;
	GLfloat lxTrans = 0.3f;
	glBegin(GL_QUADS);
	glColor3f(0.75f,0.75f,0.75f);
	glVertex3f(-lxTrans,lyTrans,-lzTrans);
	glVertex3f(-lxTrans,-lyTrans,lzTrans);
	glVertex3f(lxTrans,-lyTrans,lzTrans);
	glVertex3f(lxTrans,lyTrans,-lzTrans);
	glEnd();
}

void drawHorizontalRoad(){
	GLfloat lzTrans = 0.3f;
	GLfloat lyTrans = 0.05f;
	GLfloat lxTrans = 1.0f;
	glBegin(GL_QUADS);
	glColor3f(0.75f,0.75f,0.75f);
	glVertex3f(-lxTrans,-lyTrans,lzTrans);
	glVertex3f(-lxTrans,lyTrans,-lzTrans);
	glVertex3f(lxTrans,lyTrans,-lzTrans);
	glVertex3f(lxTrans,-lyTrans,lzTrans);
	glEnd();
}

void drawThirdQuadrantGreenArea(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(-1.0f,-0.1f,1.0f);
	glVertex3f(-0.3f,-0.1f,1.0f);
	glVertex3f(-0.3f,-0.05f,0.3f);
	glVertex3f(-1.0f,-0.05f,0.3f);
	glEnd();
}

void drawFourthQuadrantGreenArea(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(1.0f,-0.1f,1.0f);
	glVertex3f(0.3f,-0.1f,1.0f);
	glVertex3f(0.3f,-0.05f,0.3f);
	glVertex3f(1.0f,-0.05f,0.3f);
	glEnd();
}

void drawFirstQuadrantGreenArea(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(1.0f,0.1f,-1.0f);
	glVertex3f(0.3f,0.1f,-1.0f);
	glVertex3f(0.3f,0.05f,-0.3f);
	glVertex3f(1.0f,0.05f,-0.3f);
	glEnd();
}


void drawSecondQuadrantGreenArea(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(-1.0f,0.1f,-1.0f);
	glVertex3f(-0.3f,0.1f,-1.0f);
	glVertex3f(-0.3f,0.05f,-0.3f);
	glVertex3f(-1.0f,0.05f,-0.3f);
	glEnd();
}

void drawBuildingInFirstQuadrant(){
	void drawLeftPartOfBuilding(void);
	void drawRightPartOfBuilding(void);
	void drawMiddlePartOfBuilding(void);
	void drawBuildingTriangles(void);
	void drawWallsOfBuildingInFirstQuadrant(void);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zTrans);
	drawLeftPartOfBuilding();
	drawRightPartOfBuilding();
	drawMiddlePartOfBuilding();
	drawBuildingTriangles();
	drawWallsOfBuildingInFirstQuadrant();
}

void drawLeftPartOfBuilding(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.50f);
	glVertex3f(0.35f,0.0f,-0.4f);
	glVertex3f(0.55f,0.0f,-0.4f);
	glVertex3f(0.55f,0.5f,-0.4f);
	glVertex3f(0.35f,0.5f,-0.4f);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.20f);
	glVertex3f(0.40f,0.2f,-0.4f);
	glVertex3f(0.50f,0.2f,-0.4f);
	glVertex3f(0.50f,0.4f,-0.4f);
	glVertex3f(0.40f,0.4f,-0.4f);
	glEnd();
}

void drawRightPartOfBuilding(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.50f);
	glVertex3f(0.8f,0.0f,-0.4f);
	glVertex3f(1.0f,0.0f,-0.4f);
	glVertex3f(1.0f,0.5f,-0.4f);
	glVertex3f(0.8f,0.5f,-0.4f);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.20f);
	glVertex3f(0.85f,0.2f,-0.4f);
	glVertex3f(0.95f,0.2f,-0.4f);
	glVertex3f(0.95f,0.4f,-0.4f);
	glVertex3f(0.85f,0.4f,-0.4f);
	glEnd();
}

void drawMiddlePartOfBuilding(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.50f);
	glVertex3f(0.55f,0.2f,-0.4f);
	glVertex3f(0.8f,0.2f,-0.4f);
	glVertex3f(0.8f,0.7f,-0.4f);
	glVertex3f(0.55f,0.7f,-0.4f);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.50f);
	glVertex3f(0.60f,0.25f,-0.4f);
	glVertex3f(0.63f,0.25f,-0.4f);
	glVertex3f(0.63f,0.6f,-0.4f);
	glVertex3f(0.60f,0.6f,-0.4f);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.50f);
	glVertex3f(0.65f,0.25f,-0.4f);
	glVertex3f(0.77f,0.25f,-0.4f);
	glVertex3f(0.77f,0.6f,-0.4f);
	glVertex3f(0.65f,0.6f,-0.4f);
	glEnd();
}

void drawBuildingTriangles(){
		glBegin(GL_TRIANGLES);
		glColor3f(0.50f,0.0f,0.0f);
		//left
		glVertex3f(0.35f,0.5f,-0.4f);
		glVertex3f(0.55f,0.5f,-0.4f);
		glVertex3f((0.35f+0.55f)/2,0.7f,-0.4f);
		//right
		glVertex3f(0.8f,0.5f,-0.4f);
		glVertex3f(1.0f,0.5f,-0.4f);
		glVertex3f((0.8f+1.0f)/2,0.7f,-0.4f);
		
		//middle
		glVertex3f(0.55f,0.7f,-0.4f);
		glVertex3f(0.8f,0.7f,-0.4f);
		glVertex3f((0.55f+0.8f)/2,0.9f,-0.4f);
		glEnd();
}

void drawWallsOfBuildingInFirstQuadrant(){
	//left wall
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.0f);
	glVertex3f(0.35f,0.0f,-0.4f);
	glVertex3f(0.35f,0.5f,-0.4f);
	glVertex3f(0.35f,0.5f,-0.9f);
	glVertex3f(0.35f,0.0f,-0.9f);
	glEnd();
	
	//right wall
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.0f);
	glVertex3f(1.0f,0.0f,-0.4f);
	glVertex3f(1.0f,0.5f,-0.4f);
	glVertex3f(1.0f,0.5f,-0.9f);
	glVertex3f(1.0f,0.0f,-0.9f);
	glEnd();
	
	//top wall
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.0f);
	glVertex3f(0.35f,0.5f,-0.4f);
	glVertex3f(1.0f,0.5f,-0.4f);
	glVertex3f(1.0f,0.5f,-0.9f);
	glVertex3f(0.35f,0.5f,-0.9f);
	
	//back wall
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.25f,0.0f);
	glVertex3f(0.35f,0.5f,-0.9f);
	glVertex3f(0.35f,0.0f,-0.9f);
	glVertex3f(1.0f,0.0f,-0.9f);
	glVertex3f(1.0f,0.5f,-0.9f);
	glEnd();
}

void drawBuildingInSecondQuadrant(){
	void drawLeftPartOfBuildingInS(void);
	void drawRightPartOfBuildingInS(void);
	void drawMiddlePartOfBuildingInS(void);
	void drawBuildingTrianglesInS(void);
	void drawWallsOfBuildingInSecondQuadrant(void);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zTrans);
	drawLeftPartOfBuildingInS();
	drawRightPartOfBuildingInS();
	drawMiddlePartOfBuildingInS();
	drawBuildingTrianglesInS();
	drawWallsOfBuildingInSecondQuadrant();
}

void drawLeftPartOfBuildingInS(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.50f);
	glVertex3f(-0.35f,0.0f,-0.4f);
	glVertex3f(-0.55f,0.0f,-0.4f);
	glVertex3f(-0.55f,0.5f,-0.4f);
	glVertex3f(-0.35f,0.5f,-0.4f);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.20f);
	glVertex3f(-0.40f,0.2f,-0.4f);
	glVertex3f(-0.50f,0.2f,-0.4f);
	glVertex3f(-0.50f,0.4f,-0.4f);
	glVertex3f(-0.40f,0.4f,-0.4f);
	glEnd();
}

void drawRightPartOfBuildingInS(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.50f);
	glVertex3f(-0.8f,0.0f,-0.4f);
	glVertex3f(-1.0f,0.0f,-0.4f);
	glVertex3f(-1.0f,0.5f,-0.4f);
	glVertex3f(-0.8f,0.5f,-0.4f);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.20f);
	glVertex3f(-0.85f,0.2f,-0.4f);
	glVertex3f(-0.95f,0.2f,-0.4f);
	glVertex3f(-0.95f,0.4f,-0.4f);
	glVertex3f(-0.85f,0.4f,-0.4f);
	glEnd();
}

void drawMiddlePartOfBuildingInS(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.50f);
	glVertex3f(-0.55f,0.2f,-0.4f);
	glVertex3f(-0.8f,0.2f,-0.4f);
	glVertex3f(-0.8f,0.7f,-0.4f);
	glVertex3f(-0.55f,0.7f,-0.4f);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.50f);
	glVertex3f(-0.60f,0.25f,-0.4f);
	glVertex3f(-0.63f,0.25f,-0.4f);
	glVertex3f(-0.63f,0.6f,-0.4f);
	glVertex3f(-0.60f,0.6f,-0.4f);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.50f);
	glVertex3f(-0.65f,0.25f,-0.4f);
	glVertex3f(-0.77f,0.25f,-0.4f);
	glVertex3f(-0.77f,0.6f,-0.4f);
	glVertex3f(-0.65f,0.6f,-0.4f);
	glEnd();
}

void drawBuildingTrianglesInS(){
		glBegin(GL_TRIANGLES);
		glColor3f(0.50f,0.0f,0.0f);
		//left
		glVertex3f(-0.35f,0.5f,-0.4f);
		glVertex3f(-0.55f,0.5f,-0.4f);
		glVertex3f(-(0.35f+0.55f)/2,0.7f,-0.4f);
		//right
		glVertex3f(-0.8f,0.5f,-0.4f);
		glVertex3f(-1.0f,0.5f,-0.4f);
		glVertex3f(-(0.8f+1.0f)/2,0.7f,-0.4f);
		
		//middle
		glVertex3f(-0.55f,0.7f,-0.4f);
		glVertex3f(-0.8f,0.7f,-0.4f);
		glVertex3f(-(0.55f+0.8f)/2,0.9f,-0.4f);
		glEnd();
}

void drawWallsOfBuildingInSecondQuadrant(){
	//left wall
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.0f);
	glVertex3f(-0.35f,0.0f,-0.4f);
	glVertex3f(-0.35f,0.5f,-0.4f);
	glVertex3f(-0.35f,0.5f,-0.9f);
	glVertex3f(-0.35f,0.0f,-0.9f);
	glEnd();
	
	//right wall
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.0f);
	glVertex3f(-1.0f,0.0f,-0.4f);
	glVertex3f(-1.0f,0.5f,-0.4f);
	glVertex3f(-1.0f,0.5f,-0.9f);
	glVertex3f(-1.0f,0.0f,-0.9f);
	glEnd();
	
	//top wall
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.50f,0.0f);
	glVertex3f(-0.35f,0.5f,-0.4f);
	glVertex3f(-1.0f,0.5f,-0.4f);
	glVertex3f(-1.0f,0.5f,-0.9f);
	glVertex3f(-0.35f,0.5f,-0.9f);
	
	//back wall
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.25f,0.0f);
	glVertex3f(-0.35f,0.5f,-0.9f);
	glVertex3f(-0.35f,0.0f,-0.9f);
	glVertex3f(-1.0f,0.0f,-0.9f);
	glVertex3f(-1.0f,0.5f,-0.9f);
	glEnd();
}

void drawCar(){
	void drawCarFrontSideArea(void);
	void drawCarRearArea(void);
	void drawFWheel(void);
	void drawBWheel(void);
	void drawFRWheel(void);
	void drawBRWheel(void);
	void drawCarFrontViewArea(void);
	void drawCarBackAread(void);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zTrans);
	drawCarFrontSideArea();
	drawCarRearArea();
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zTrans);
	drawCarFrontViewArea();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zTrans);
	drawCarBackAread();
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.55f,0.0f,zTrans+0.2f);
	drawFWheel();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.7f,0.0f,zTrans+0.2f);
	drawBWheel();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.55f,0.0f,zTrans+0.2f-0.1f);
	drawFRWheel();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.7f,0.0f,zTrans+0.2f-0.1f);
	drawBRWheel();
}

void drawCarFrontSideArea(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(-0.8f,0.15f,0.0f);
	glVertex3f(-0.9f,-0.1f,0.0f);
	glVertex3f(-0.4f,-0.1f,0.0f);
	glVertex3f(-0.6f,0.15f,0.0f);
	glEnd();
	
	glBegin(GL_LINES);
	glColor3f(0.0f,0.0f,0.0f);
	//middle line
	glVertex3f(-0.84f,0.05f,0.0f);
	glVertex3f(-0.52f,0.05f,0.0f);
	
	//top
	glVertex3f(-0.8f,0.14f,0.0f);
	glVertex3f(-0.6f,0.14f,0.0f);
	
	//verticals
	glVertex3f(-0.8f,0.14f,0.0f);
	glVertex3f(-0.8f,0.05f,0.0f);
	
	glVertex3f(-0.7f,0.14f,0.0f);
	glVertex3f(-0.7f,0.05f,0.0f);
	
	glVertex3f(-0.6f,0.14f,0.0f);
	glVertex3f(-0.6f,0.05f,0.0f);
	glEnd();
}

void drawCarBackAread(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(-0.8f,0.15f,0.0f);
	glVertex3f(-0.8f,0.15f,-0.1f);
	glVertex3f(-0.9f,-0.1f,-0.1f);
	glVertex3f(-0.88f,-0.1f,0.0f);
	glEnd();
}

void drawCarFrontViewArea(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(-0.6f,0.15f,0.0f);
	glVertex3f(-0.6f,0.15f,-0.1f);
	glVertex3f(-0.45f,-0.1f,-0.1f);
	glVertex3f(-0.45f,-0.1f,0.0f);
	glEnd();
}

void drawCarRearArea(){
	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(-0.8f,0.15f,-0.1f);
	glVertex3f(-0.9f,-0.1f,-0.1f);
	glVertex3f(-0.4f,-0.1f,-0.1f);
	glVertex3f(-0.6f,0.15f,-0.1f);
	glEnd();
	
	glBegin(GL_LINES);
	glColor3f(0.0f,0.0f,0.0f);
	//middle line
	glVertex3f(-0.84f,0.05f,-0.1f);
	glVertex3f(-0.52f,0.05f,-0.1f);
	
	//top
	glVertex3f(-0.8f,0.14f,-0.1f);
	glVertex3f(-0.6f,0.14f,-0.1f);
	
	//verticals
	glVertex3f(-0.8f,0.14f,-0.1f);
	glVertex3f(-0.8f,0.05f,-0.1f);
	
	glVertex3f(-0.7f,0.14f,-0.1f);
	glVertex3f(-0.7f,0.05f,-0.1f);
	
	glVertex3f(-0.6f,0.14f,-0.1f);
	glVertex3f(-0.6f,0.05f,-0.1f);
	glEnd();
}

void drawFWheel(){
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f,0.0f,0.0f);
	for(float angle=0.0f;angle<(2.0f*PI); angle = angle + 0.01){
		glVertex3f(cos(angle)*CIRCLE_SIZE,sin(angle)*CIRCLE_SIZE,0.0f);
	}
	glEnd();
}

void drawBWheel(){
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f,0.0f,0.0f);
	for(float angle=0.0f;angle<(2.0f*PI); angle = angle + 0.01){
		glVertex3f(cos(angle)*CIRCLE_SIZE,sin(angle)*CIRCLE_SIZE,0.0f);
	}
	glEnd();
}

void drawFRWheel(){
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f,0.0f,0.0f);
	for(float angle=0.0f;angle<(2.0f*PI); angle = angle + 0.01){
		glVertex3f(cos(angle)*CIRCLE_SIZE,sin(angle)*CIRCLE_SIZE,0.0f);
	}
	glEnd();
}

void drawBRWheel(){
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f,0.0f,0.0f);
	for(float angle=0.0f;angle<(2.0f*PI); angle = angle + 0.01){
		glVertex3f(cos(angle)*CIRCLE_SIZE,sin(angle)*CIRCLE_SIZE,0.0f);
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