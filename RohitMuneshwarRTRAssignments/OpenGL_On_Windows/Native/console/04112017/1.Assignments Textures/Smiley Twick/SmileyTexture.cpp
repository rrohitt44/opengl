#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>

#include "SmileyTexture.h"

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
float rotateAngle=0.0f;
float speed =0.0f;
float rotateX=0.0f;
float rotateY=0.0f;
float rotateZ=0.0f;
int gNumKeyPressed=0;

GLuint Texture_Smiley;

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
	wsprintf(windowTitle,TEXT("ROHIT: Texture : Perspective zoom press i or o : %d"),zoomInOut);
	
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
			rotate();
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
					setWindowTitle(TEXT("ROHIT: Texture: Perspective zoom : %d "),zoomInOut);
				break;
				case 0x4F: //o
				    zoomInOut= zoomInOut-1.0f;
					setWindowTitle(TEXT("ROHIT: Texture: Perspective zoom : %d "),zoomInOut);
				break;
				case 0x53://s
					if(speed==1.0f)
						speed=0.0f;
				
					speed= speed+0.10f;
				break;
				case 0x58: //x
					if(rotateX==0.0f)
						rotateX=1.0f;
					else 
						rotateX=0.0f;
					setWindowTitle(TEXT("ROHIT: Rotate on X:%d Y:%d Z:%d "),rotateX,rotateY,rotateZ);
					
				break;
				case 0x59://y
					if(rotateY==0.0f)
						rotateY=1.0f;
					else 
						rotateY=0.0f;
					setWindowTitle(TEXT("ROHIT: Rotate on X:%d Y:%d Z:%d "),rotateX,rotateY,rotateZ);
				
				break;
				case 0x5A: //z
					if(rotateZ==0.0f)
						rotateZ=1.0f;
					else 
						rotateZ=0.0f;
					setWindowTitle(TEXT("ROHIT: Rotate on X:%d Y:%d Z:%d "),rotateX,rotateY,rotateZ);
				break;
				case 0x31: //1  
					gNumKeyPressed=1;
				break;
				case 0x32: //2  
					gNumKeyPressed=2;
				break;
				case 0x33: //3  
					gNumKeyPressed=3;
				break;
				case 0x34: //4  
					gNumKeyPressed=4;
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
	int LoadGLTextures(GLuint *,TCHAR[]);
	
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
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glShadeModel(GL_SMOOTH);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	
	glEnable(GL_TEXTURE_2D);
	LoadGLTextures(&Texture_Smiley,MAKEINTRESOURCE(IDBITMAP_SMILEY));

	resize(WIN_WIDTH,WIN_HEIGHT);
}

int LoadGLTextures(GLuint *texture,TCHAR imageResourceID[]){
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = FALSE;

	glGenTextures(1,texture);
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),imageResourceID,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);
	if(hBitmap){
		iStatus= TRUE;
		GetObject(hBitmap,sizeof(bmp),&bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		glBindTexture(GL_TEXTURE_2D,*texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D,3,bmp.bmWidth,bmp.bmHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,bmp.bmBits);
		DeleteObject(hBitmap);
	}
	return iStatus;
}
void uninitialize(void){
	
	if(Texture_Smiley){
		glDeleteTextures(1,&Texture_Smiley);
		Texture_Smiley=0;
	}

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
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,zoomInOut);
	glRotatef(rotateAngle,rotateX,rotateY,rotateZ);
	glBindTexture(GL_TEXTURE_2D,Texture_Smiley);

	glBegin(GL_QUADS);
	
	// front
	if(gNumKeyPressed==1){
		glTexCoord2f(0.5f,0.5f);
		glVertex3f(1.0f,1.0f,1.0f);  //rt

		glTexCoord2f(0.0f,0.5f);
		glVertex3f(-1.0f,1.0f,1.0f);  //lt

		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);  //lb

		glTexCoord2f(0.5f,0.0f);
		glVertex3f(1.0f,-1.0f,1.0f);  //rt
	} else if(gNumKeyPressed==2){
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);  //rt

		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-1.0f,1.0f,1.0f);  //lt

		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);  //lb

		glTexCoord2f(1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,1.0f);  //rt
	} else if(gNumKeyPressed==3){
		glTexCoord2f(2.0f,2.0f);
		glVertex3f(1.0f,1.0f,1.0f);  //rt

		glTexCoord2f(0.0f,2.0f);
		glVertex3f(-1.0f,1.0f,1.0f);  //lt

		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);  //lb

		glTexCoord2f(2.0f,0.0f);
		glVertex3f(1.0f,-1.0f,1.0f);  //rt
	} else if(gNumKeyPressed==4){
		glTexCoord2f(0.5f,0.5f);
		glVertex3f(1.0f,1.0f,1.0f);  //rt

		glTexCoord2f(0.5f,0.5f);
		glVertex3f(-1.0f,1.0f,1.0f);  //lt

		glTexCoord2f(0.5f,0.5f);
		glVertex3f(-1.0f,-1.0f,1.0f);  //lb
		
		glTexCoord2f(0.5f,0.5f);
		glVertex3f(1.0f,-1.0f,1.0f);  //rt
	}
	// right
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,1.0f);  //rt

	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);  //lt

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);  //rt

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);  //rt

	// back
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,1.0f,-1.0f);  //rt

	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);  //lt

	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);  //lb

	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);  //rt

	// left
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f); // RT+z
	
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f); //RT-z
	
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(1.0f,-1.0f,-1.0f); //LB+z	
	
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,1.0f,-1.0f); //LB-Z

	// top
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f); // RT+z
	
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,1.0f,-1.0f); //RT-z
	
	glTexCoord2f(0.0f,0.0f);	
	glVertex3f(-1.0f,1.0f,-1.0f); //LT-z
	
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,1.0f,1.0f); //LT+z
	
	// bottom
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f); // RB+z
	
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f); //RB-z
	
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f); //LB-z
	
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,1.0f); //LB+z

	glEnd();

	SwapBuffers(ghdc);
}

void rotate(){
		

	if(rotateAngle==360.0f)
		rotateAngle=0.0f;
	rotateAngle = rotateAngle + speed;
}


void resize(int width,int height){
	if(height==0)
		height=1;
	
	glViewport(0,0,(GLsizei)width, (GLsizei)height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if(width<=height)
		gluPerspective(45.0f,(GLfloat)height/(GLfloat)width,0.1f,100.0f);
	else
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

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