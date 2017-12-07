#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>

#define CheckImageWidth 64
#define CheckImageHeight 64

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC  ghrc=NULL;

bool gbEscapedKeyIsPressed = false;
bool gbActiveWindow = false;
bool gbFullScreen = false;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};

int zoomInOut=-10;

GLubyte checkImage[CheckImageHeight][CheckImageWidth][4];
GLuint texName;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR sziCmdShow,int iCmdShow){
	
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[255] = TEXT("OpenGL");	
	bool bDone = false;
	WNDCLASSEX wndClass;
	
	void display(void);
	void initialize(void);
	void uninitialize(void);
	void rotate(void);
	
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance,TEXT("MYICO"));
	wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;
	wndClass.hIconSm = LoadIcon(hInstance,TEXT("MYICO"));
	
	RegisterClassEx(&wndClass);
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
	
	TCHAR windowTitle[255];
	wsprintf(windowTitle,TEXT("Rohit: Texture CheckBoard: Perspective zoom press i or o : %d"),zoomInOut);
	
	hwnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			szAppName,
			windowTitle,
			WS_OVERLAPPEDWINDOW  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
			(rect.right - rect.left) / 2 - WIN_WIDTH/2,
			(rect.bottom - rect.top) / 2 - WIN_HEIGHT/2,
			WIN_WIDTH, 
			WIN_HEIGHT,
			NULL ,
			NULL ,
			hInstance,
			NULL
			);
	ghwnd = hwnd;
	
	initialize();
	
	ShowWindow(hwnd,SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	
	while( bDone == false ){
		
		if( PeekMessage(&msg,NULL,0,0,PM_REMOVE) ){
			
			if(msg.message == WM_QUIT)
				bDone = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			
			if(gbActiveWindow == true){
				if(gbEscapedKeyIsPressed==true)
					bDone = true;
			}
			
			display();
		
		}
	} 
	uninitialize();
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void resize(int,int);
	void ToggleFullScreen(void);
	void uninitialize(void);
	void setWindowTitle(LPCTSTR,int);
	void setWindowTitle(LPCTSTR,int,int,int);

	switch(iMsg){
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0)
				gbActiveWindow = true;
			else 
				gbActiveWindow = false;
		break;
		case WM_KEYDOWN:
			switch(wParam){
				case 0x1B:
					if (gbEscapedKeyIsPressed == false)
						gbEscapedKeyIsPressed = true; 
				break;
				case 0x46:
					ToggleFullScreen();
				break;
				case 0x49: //i
					zoomInOut=zoomInOut+1.0f;
					setWindowTitle(TEXT("Rohit: Texture: Perspective zoom : %d "),zoomInOut);
				break;
				case 0x4F: //o
				    zoomInOut= zoomInOut-1.0f;
					setWindowTitle(TEXT("Rohit: Texture: Perspective zoom : %d "),zoomInOut);
				break;				
				
			}
		break;
		case WM_SIZE:
			resize(LOWORD(lParam),HIWORD(lParam));
		break;
		case WM_CLOSE:
			uninitialize();
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		
	}
	
	return DefWindowProc(hwnd,iMsg,wParam,lParam);
}
void setWindowTitle(LPCTSTR text,int zoom){
	TCHAR windowTitle[255];
	wsprintf(windowTitle,text,zoom);	
	SetWindowText(ghwnd,windowTitle);
}
void setWindowTitle(LPCTSTR text,int x,int y,int z){
	TCHAR windowTitle[255];
	wsprintf(windowTitle,text,x,y,z);	
	SetWindowText(ghwnd,windowTitle);
}
void initialize(void){
	void resize(int,int);
	void LoadTexture();
	
	PIXELFORMATDESCRIPTOR pfd;
	
	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));
	
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER ;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	
	ghdc = GetDC(ghwnd);
	
	if(ghdc==NULL){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}
	
	int iPixelFormatIndex = ChoosePixelFormat(ghdc,&pfd);
	
	if(iPixelFormatIndex==0){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}
	
	if(SetPixelFormat(ghdc,iPixelFormatIndex,&pfd) == FALSE){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}
	
	ghrc = wglCreateContext(ghdc);
	if(ghrc==NULL){
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}
	if(wglMakeCurrent(ghdc,ghrc)== FALSE){
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}
	
	//clear color
	glClearColor(0.0f,0.0f,0.0f,0.0f); //black

	//shade model
	glShadeModel(GL_FLAT);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	//texture
	glEnable(GL_TEXTURE_2D);
	LoadTexture();


	resize(WIN_WIDTH,WIN_HEIGHT);
}

void LoadTexture(){
	void MakeCheckImage();

	MakeCheckImage();

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texName);

	glBindTexture(GL_TEXTURE_2D,texName);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,CheckImageWidth,CheckImageHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,checkImage);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

void MakeCheckImage(){
	int i, j, c;

	for(int i=0;i<CheckImageHeight;i++){
		for(int j=0;j<CheckImageWidth;j++){
			c= (((i & 0x8) ==0) ^ ((j & 0x8) ==0))*255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}
void uninitialize(void){
	
	
	glEnable(GL_TEXTURE_2D);
	glDeleteTextures(1,&texName);

	if(gbFullScreen==true){
		SetWindowLong(ghwnd,GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOSIZE | SWP_NOMOVE| SWP_NOOWNERZORDER| SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc=NULL;
	ReleaseDC(ghwnd,ghdc);
	ghdc=NULL;
	DestroyWindow(ghwnd);
}
void display(void){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	glTranslatef(0.0f,0.0f,(GLfloat)-3.6);
	glBegin(GL_QUADS);
		//straight facing quad
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-2.0f,-1.0f,0.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-2.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(0.0f,-1.0f,0.0f);

		//tilted quad
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(1.0f,-1.0f,0.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(2.41421f,1.0f,-1.41421f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(2.41421f,-1.0f,-1.41421f);
	glEnd();

	SwapBuffers(ghdc);
}


void resize(int width,int height){
	if(height==0)
		height=1;
	
	glViewport(0,0,(GLsizei)width, (GLsizei)height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if(width<=height)
		gluPerspective(45.0f,(GLfloat)height/(GLfloat)width,1.0f,30.0f);
	else
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,30.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void ToggleFullScreen(void){
	
	if(gbFullScreen==false){
		
		dwStyle = GetWindowLong(ghwnd,GWL_STYLE);
		
		if(dwStyle & WS_OVERLAPPEDWINDOW){
			
			bool fwinPlacement = GetWindowPlacement(ghwnd,&wpPrev);
			
			MONITORINFO mi = {sizeof(MONITORINFO)};
			
			HMONITOR hMonitor = MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY);
			
			bool fMonitorInfo = GetMonitorInfo( hMonitor,&mi);
			
			if(fwinPlacement && fMonitorInfo){
				SetWindowLong(ghwnd,GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen=true;
	}else {
			SetWindowLong(ghwnd,GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
			SetWindowPlacement(ghwnd,&wpPrev);
			SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOSIZE | SWP_NOMOVE| SWP_NOOWNERZORDER| SWP_NOZORDER | SWP_FRAMECHANGED);
			ShowCursor(TRUE);
		gbFullScreen=false;
	}
	
}
