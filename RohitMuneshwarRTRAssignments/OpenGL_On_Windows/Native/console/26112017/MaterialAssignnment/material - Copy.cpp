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

GLfloat gfAngleS1 = 0.0f;
GLfloat gfAngleS2 = 0.0f;
GLfloat gfAngleS3 = 0.0f;
GLfloat gfAngleS4 = 0.0f;
GLfloat gfAngleS5 = 0.0f;
GLfloat gfAngleS6 = 0.0f;
GLfloat gfAngleS7 = 0.0f;
GLfloat gfAngleS8 = 0.0f;
GLfloat gfAngleS9 = 0.0f;
GLfloat gfAngleS10 = 0.0f;
GLfloat gfAngleS11 = 0.0f;
GLfloat gfAngleS12 = 0.0f;
GLfloat gfAngleS13 = 0.0f;
GLfloat gfAngleS14 = 0.0f;
GLfloat gfAngleS15 = 0.0f;
GLfloat gfAngleS16 = 0.0f;
GLfloat gfAngleS17 = 0.0f;
GLfloat gfAngleS18 = 0.0f;
GLfloat gfAngleS19 = 0.0f;
GLfloat gfAngleS20 = 0.0f;
GLfloat gfAngleS21 = 0.0f;
GLfloat gfAngleS22 = 0.0f;
GLfloat gfAngleS23 = 0.0f;
GLfloat gfAngleS24 = 0.0f;

GLUquadric *quadric = NULL;

GLfloat light_model_ambient[]={0.2f,0.2f,0.2f,0.0f};
GLfloat light_model_local_viewer[]={0.0f};

GLfloat light_ambient[]={0.5f,0.5f,0.5f,1.0f};
GLfloat light_defused[]={1.0f,1.0f,1.0f,1.0f};
GLfloat light_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat light_position[]={0.0f,0.0f,1.0f,0.0f};
GLfloat material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_shinnyness[]={50.0f};

GLfloat s1_material_ambient[]={0.0215f,0.1745f,0.0215f,1.0f};
GLfloat s1_material_diffuse[]={0.07568f,0.61424f,0.07568f,1.0f};
GLfloat s1_material_specular[]={0.633f,0.727811f,0.633f,1.0f};
GLfloat s1_material_shininess[]={0.6f * 128};

GLfloat s2_material_ambient[]={0.135f,0.2225f,0.1575f,1.0f};
GLfloat s2_material_diffuse[]={0.54f,0.89f,0.63f,1.0f};
GLfloat s2_material_specular[]={0.316228f,0.316228f,0.316228f,1.0f};
GLfloat s2_material_shininess[]={0.1f * 128};

GLfloat s3_material_ambient[]={0.05375f,0.05f,0.06625f,1.0f};
GLfloat s3_material_diffuse[]={0.18275f,0.17f,0.22525f,1.0f};
GLfloat s3_material_specular[]={0.332741f,0.328634f,0.346435f,1.0f};
GLfloat s3_material_shininess[]={0.3f * 128};

GLfloat s4_material_ambient[]={0.25f,0.20725f,0.20725f,1.0f};
GLfloat s4_material_diffuse[]={1.0f,0.829f,0.829f,1.0f};
GLfloat s4_material_specular[]={0.296648f,0.296648f,0.296648f,1.0f};
GLfloat s4_material_shininess[]={0.088f * 128};

GLfloat s5_material_ambient[]={0.1745f,0.01175f,0.01175f,1.0f};
GLfloat s5_material_diffuse[]={0.61424f,0.04136f,0.04136f,1.0f};
GLfloat s5_material_specular[]={0.727811f,0.626959f,0.626959f,1.0f};
GLfloat s5_material_shininess[]={0.6f * 128};

GLfloat s6_material_ambient[]={0.1f,0.18725f,0.1745f,1.0f};
GLfloat s6_material_diffuse[]={0.396f,0.74151f,0.69102f,1.0f};
GLfloat s6_material_specular[]={0.297254f,0.30829f,0.306678f,1.0f};
GLfloat s6_material_shininess[]={0.1f * 128};

GLfloat s7_material_ambient[]={0.329412f,0.223529f,0.027451f,1.0f};
GLfloat s7_material_diffuse[]={0.780392f,0.568627f,0.113725f,1.0f};
GLfloat s7_material_specular[]={0.992157f,0.941176f,0.807843f,1.0f};
GLfloat s7_material_shininess[]={0.21794872f * 128};

GLfloat s8_material_ambient[]={0.2125f,0.1275f,0.054f,1.0f};
GLfloat s8_material_diffuse[]={0.714f,0.4284f,0.18144f,1.0f};
GLfloat s8_material_specular[]={0.393548f,0.271906f,0.166721f,1.0f};
GLfloat s8_material_shininess[]={0.2f * 128};

GLfloat s9_material_ambient[]={0.25f,0.25f,0.25f,1.0f};
GLfloat s9_material_diffuse[]={0.4f,0.4f,0.4f,1.0f};
GLfloat s9_material_specular[]={0.774597f,0.774597f,0.774597f,1.0f};
GLfloat s9_material_shininess[]={0.6f * 128};

GLfloat s10_material_ambient[]={0.19125f,0.0735f,0.0225f,1.0f};
GLfloat s10_material_diffuse[]={0.7038f,0.27048f,0.0828f,1.0f};
GLfloat s10_material_specular[]={0.256777f,0.137622f,0.086014f,1.0f};
GLfloat s10_material_shininess[]={0.1f * 128};

GLfloat s11_material_ambient[]={0.24725f,0.1995f,0.0745f,1.0f};
GLfloat s11_material_diffuse[]={0.75164f,0.60648f,0.22648f,1.0f};
GLfloat s11_material_specular[]={0.628281f,0.555802f,0.366065f,1.0f};
GLfloat s11_material_shininess[]={0.4f * 128};

GLfloat s12_material_ambient[]={0.19225f,0.19225f,0.19225f,1.0f};
GLfloat s12_material_diffuse[]={0.50754f,0.50754f,0.50754f,1.0f};
GLfloat s12_material_specular[]={0.508273f,0.508273f,0.508273f,1.0f};
GLfloat s12_material_shininess[]={0.4f * 128};

GLfloat s13_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s13_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
GLfloat s13_material_specular[]={0.50f,0.50f,0.50f,1.0f};
GLfloat s13_material_shininess[]={0.25f * 128};

GLfloat s14_material_ambient[]={0.0f,0.1f,0.06f,1.0f};
GLfloat s14_material_diffuse[]={0.0f,0.50980392f,0.50980392f,1.0f};
GLfloat s14_material_specular[]={0.50196078f,0.50196078f,0.50196078f,1.0f};
GLfloat s14_material_shininess[]={0.25f * 128};

GLfloat s15_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s15_material_diffuse[]={0.1f,0.35f,0.1f,1.0f};
GLfloat s15_material_specular[]={0.45f,0.55f,0.45f,1.0f};
GLfloat s15_material_shininess[]={0.25f * 128};

GLfloat s16_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s16_material_diffuse[]={0.5f,0.0f,0.0f,1.0f};
GLfloat s16_material_specular[]={0.7f,0.6f,0.6f,1.0f};
GLfloat s16_material_shininess[]={0.25f * 128};

GLfloat s17_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s17_material_diffuse[]={0.55f,0.55f,0.55f,1.0f};
GLfloat s17_material_specular[]={0.70f,0.70f,0.70f,1.0f};
GLfloat s17_material_shininess[]={0.25f * 128};

GLfloat s18_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s18_material_diffuse[]={0.5f,0.5f,0.0f,1.0f};
GLfloat s18_material_specular[]={0.60f,0.60f,0.50f,1.0f};
GLfloat s18_material_shininess[]={0.25f * 128};

GLfloat s19_material_ambient[]={0.02f,0.02f,0.02f,1.0f};
GLfloat s19_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
GLfloat s19_material_specular[]={0.4f,0.4f,0.4f,1.0f};
GLfloat s19_material_shininess[]={0.078125f * 128};

GLfloat s20_material_ambient[]={0.0f,0.05f,0.05f,1.0f};
GLfloat s20_material_diffuse[]={0.4f,0.5f,0.5f,1.0f};
GLfloat s20_material_specular[]={0.04f,0.7f,0.7f,1.0f};
GLfloat s20_material_shininess[]={0.078125f * 128};

GLfloat s21_material_ambient[]={0.0f,0.05f,0.0f,1.0f};
GLfloat s21_material_diffuse[]={0.4f,0.5f,0.4f,1.0f};
GLfloat s21_material_specular[]={0.04f,0.7f,0.04f,1.0f};
GLfloat s21_material_shininess[]={0.078125f * 128};

GLfloat s22_material_ambient[]={0.05f,0.0f,0.0f,1.0f};
GLfloat s22_material_diffuse[]={0.5f,0.4f,0.4f,1.0f};
GLfloat s22_material_specular[]={0.7f,0.04f,0.04f,1.0f};
GLfloat s22_material_shininess[]={0.078125f * 128};

GLfloat s23_material_ambient[]={0.05f,0.05f,0.05f,1.0f};
GLfloat s23_material_diffuse[]={0.5f,0.5f,0.5f,1.0f};
GLfloat s23_material_specular[]={0.7f,0.7f,0.7f,1.0f};
GLfloat s23_material_shininess[]={0.078125f * 128};

GLfloat s24_material_ambient[]={0.05f,0.05f,0.0f,1.0f};
GLfloat s24_material_diffuse[]={0.5f,0.5f,0.4f,1.0f};
GLfloat s24_material_specular[]={0.7f,0.7f,0.04f,1.0f};
GLfloat s24_material_shininess[]={0.078125f * 128};

bool gbLightMode = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLIne, int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);//double buffer sathi
	void updateAngle(void); 
	WNDCLASSEX wndclass;
	TCHAR AppName[] = TEXT("Window Custom");
	TCHAR WinName[] = TEXT("Lights - Materials");
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
	gfAnglePira = gfAnglePira + 0.1f;
	if(gfAnglePira>=360.0f){
		gfAnglePira = 0.0f;
	}
	
	gfAngleCube = gfAngleCube + 0.1f;
	if(gfAngleCube>=360.0f){
		gfAngleCube = 0.0f;
	}
	
	gfAngleS1 = gfAngleS1 + 0.1f;
	if(gfAngleS1>=360.0f){
		gfAngleS1 = 0.0f;
	}
	
	gfAngleS2 = gfAngleS2 + 0.1f;
	if(gfAngleS2>=360.0f){
		gfAngleS2 = 0.0f;
	}
	
	gfAngleS3 = gfAngleS3 + 0.1f;
	if(gfAngleS3>=360.0f){
		gfAngleS3 = 0.0f;
	}
	
	gfAngleS4 = gfAngleS4 + 0.1f;
	if(gfAngleS4>=360.0f){
		gfAngleS4 = 0.0f;
	}
	
	gfAngleS5 = gfAngleS5 + 0.1f;
	if(gfAngleS5>=360.0f){
		gfAngleS5 = 0.0f;
	}
	
	gfAngleS6 = gfAngleS6 + 0.1f;
	if(gfAngleS6>=360.0f){
		gfAngleS6 = 0.0f;
	}
	
	gfAngleS7 = gfAngleS7 + 0.1f;
	if(gfAngleS7>=360.0f){
		gfAngleS7 = 0.0f;
	}
	
	gfAngleS8 = gfAngleS8 + 0.1f;
	if(gfAngleS8>=360.0f){
		gfAngleS8 = 0.0f;
	}
	
	gfAngleS9 = gfAngleS9 + 0.1f;
	if(gfAngleS9>=360.0f){
		gfAngleS9 = 0.0f;
	}
	
	gfAngleS10 = gfAngleS10 + 0.1f;
	if(gfAngleS10>=360.0f){
		gfAngleS10 = 0.0f;
	}
	
	gfAngleS11 = gfAngleS11 + 0.1f;
	if(gfAngleS11>=360.0f){
		gfAngleS11 = 0.0f;
	}
	
	gfAngleS12 = gfAngleS12 + 0.1f;
	if(gfAngleS12>=360.0f){
		gfAngleS12 = 0.0f;
	}
	
	gfAngleS13 = gfAngleS13 + 0.1f;
	if(gfAngleS13>=360.0f){
		gfAngleS13 = 0.0f;
	}
	
	gfAngleS14 = gfAngleS14 + 0.1f;
	if(gfAngleS14>=360.0f){
		gfAngleS14 = 0.0f;
	}
	
	gfAngleS15 = gfAngleS15 + 0.1f;
	if(gfAngleS15>=360.0f){
		gfAngleS15 = 0.0f;
	}
	
	gfAngleS16 = gfAngleS16 + 0.1f;
	if(gfAngleS16>=360.0f){
		gfAngleS16 = 0.0f;
	}
	
	gfAngleS17 = gfAngleS17 + 0.1f;
	if(gfAngleS17>=360.0f){
		gfAngleS17 = 0.0f;
	}
	
	gfAngleS18 = gfAngleS18 + 0.1f;
	if(gfAngleS18>=360.0f){
		gfAngleS18 = 0.0f;
	}
	
	gfAngleS19 = gfAngleS19 + 0.1f;
	if(gfAngleS19>=360.0f){
		gfAngleS19 = 0.0f;
	}
	
	gfAngleS20 = gfAngleS20 + 0.1f;
	if(gfAngleS20>=360.0f){
		gfAngleS20 = 0.0f;
	}
	
	gfAngleS21 = gfAngleS21 + 0.1f;
	if(gfAngleS21>=360.0f){
		gfAngleS21 = 0.0f;
	}
	
	gfAngleS22 = gfAngleS22 + 0.1f;
	if(gfAngleS22>=360.0f){
		gfAngleS22 = 0.0f;
	}
	
	gfAngleS23 = gfAngleS23 + 0.1f;
	if(gfAngleS23>=360.0f){
		gfAngleS23 = 0.0f;
	}
	
	gfAngleS24 = gfAngleS24 + 0.1f;
	if(gfAngleS24>=360.0f){
		gfAngleS24 = 0.0f;
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
				case 0x4C://L
					toggleLightMode();
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
	
	glClearColor(0.25f,0.25f,0.25f,0.0f);
	
	//added for 3D support
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,light_model_local_viewer);
	//added for 3D support but are optional
	//glShadeModel(GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//removed as not needed for double buffering
	
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_defused);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	
	/*
	//s1
	glMaterialfv(GL_FRONT,GL_AMBIENT,s1_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s1_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s1_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s1_material_shininess);
	
	//s2
	glMaterialfv(GL_FRONT,GL_AMBIENT,s2_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s2_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s2_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s2_material_shininess);
	
	//s3
	glMaterialfv(GL_FRONT,GL_AMBIENT,s3_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s3_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s3_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s3_material_shininess);
	
	//s4
	glMaterialfv(GL_FRONT,GL_AMBIENT,s4_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s4_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s4_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s4_material_shininess);
	
	//s5
	glMaterialfv(GL_FRONT,GL_AMBIENT,s5_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s5_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s5_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s5_material_shininess);
	
	//s6
	glMaterialfv(GL_FRONT,GL_AMBIENT,s6_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s6_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s6_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s6_material_shininess);
	
	//s7
	glMaterialfv(GL_FRONT,GL_AMBIENT,s7_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s7_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s7_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s7_material_shininess);
	
	//s8
	glMaterialfv(GL_FRONT,GL_AMBIENT,s8_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s8_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s8_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s8_material_shininess);
	
	//s9
	glMaterialfv(GL_FRONT,GL_AMBIENT,s9_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s9_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s9_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s9_material_shininess);
	
	//s10
	glMaterialfv(GL_FRONT,GL_AMBIENT,s10_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s10_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s10_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s10_material_shininess);
	
	//s11
	glMaterialfv(GL_FRONT,GL_AMBIENT,s11_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s11_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s11_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s11_material_shininess);
	
	//s12
	glMaterialfv(GL_FRONT,GL_AMBIENT,s12_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s12_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s12_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s12_material_shininess);
	
	//s13
	glMaterialfv(GL_FRONT,GL_AMBIENT,s13_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s13_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s13_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s13_material_shininess);
	
	//s14
	glMaterialfv(GL_FRONT,GL_AMBIENT,s14_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s14_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s14_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s14_material_shininess);
	
	//s15
	glMaterialfv(GL_FRONT,GL_AMBIENT,s15_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s15_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s15_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s15_material_shininess);
	
	//s16
	glMaterialfv(GL_FRONT,GL_AMBIENT,s16_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s16_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s16_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s16_material_shininess);
	
	//s17
	glMaterialfv(GL_FRONT,GL_AMBIENT,s17_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s17_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s17_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s17_material_shininess);
	
	//s18
	glMaterialfv(GL_FRONT,GL_AMBIENT,s18_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s18_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s18_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s18_material_shininess);
	
	//s19
	glMaterialfv(GL_FRONT,GL_AMBIENT,s19_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s19_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s19_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s19_material_shininess);
	
	//s20
	glMaterialfv(GL_FRONT,GL_AMBIENT,s20_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s20_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s20_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s20_material_shininess);
	
	//s21
	glMaterialfv(GL_FRONT,GL_AMBIENT,s21_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s21_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s21_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s21_material_shininess);
	
	//s22
	glMaterialfv(GL_FRONT,GL_AMBIENT,s22_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s22_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s22_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s22_material_shininess);
	
	//s23
	glMaterialfv(GL_FRONT,GL_AMBIENT,s23_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s23_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s23_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s23_material_shininess);
	
	//s24
	glMaterialfv(GL_FRONT,GL_AMBIENT,s24_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s24_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s24_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s24_material_shininess);
	*/
	glEnable(GL_LIGHT0);
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void display(){
	void DrawTriangle(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,2.0f,-6.0f);
	/*
	glBegin(GL_LINE_LOOP);
	glColor3f(0.25f,0.25f,0.25f);
	glVertex3f(-1.0f,0.3f,0.0f);
	glVertex3f(-1.0f,-2.8f,0.0f);
	glVertex3f(1.1f,-2.8f,0.0f);
	glVertex3f(1.1f,0.3f,0.0f);
	glEnd();
	*/
	
	glColor3f(1.0f,1.0f,1.0f);
	
	//first row
	//1
	//glPushMatrix();
	glTranslatef(-0.7f,0.0f,0.0f);
	
	//s1
	glMaterialfv(GL_FRONT,GL_AMBIENT,s1_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s1_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s1_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s1_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//2
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s7
	glMaterialfv(GL_FRONT,GL_AMBIENT,s7_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s7_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s7_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s7_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//3
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s13
	glMaterialfv(GL_FRONT,GL_AMBIENT,s13_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s13_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s13_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s13_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//4
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s19
	glMaterialfv(GL_FRONT,GL_AMBIENT,s19_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s19_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s19_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s19_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//second row
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,1.5f,-6.0f);
	//1
	//glPushMatrix();
	glTranslatef(-0.7f,0.0f,0.0f);
	
	//s2
	glMaterialfv(GL_FRONT,GL_AMBIENT,s2_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s2_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s2_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s2_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//2
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s8
	glMaterialfv(GL_FRONT,GL_AMBIENT,s8_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s8_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s8_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s8_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//3
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s14
	glMaterialfv(GL_FRONT,GL_AMBIENT,s14_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s14_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s14_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s14_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//4
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s20
	glMaterialfv(GL_FRONT,GL_AMBIENT,s20_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s20_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s20_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s20_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//third row
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,1.0f,-6.0f);
	//1
	//glPushMatrix();
	glTranslatef(-0.7f,0.0f,0.0f);
	
	//s3
	glMaterialfv(GL_FRONT,GL_AMBIENT,s3_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s3_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s3_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s3_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//2
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s9
	glMaterialfv(GL_FRONT,GL_AMBIENT,s9_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s9_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s9_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s9_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//3
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s15
	glMaterialfv(GL_FRONT,GL_AMBIENT,s15_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s15_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s15_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s15_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//4
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s21
	glMaterialfv(GL_FRONT,GL_AMBIENT,s21_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s21_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s21_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s21_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//fourth row
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.5f,-6.0f);
	//1
	//glPushMatrix();
	glTranslatef(-0.7f,0.0f,0.0f);
	
	//s4
	glMaterialfv(GL_FRONT,GL_AMBIENT,s4_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s4_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s4_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s4_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//2
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s10
	glMaterialfv(GL_FRONT,GL_AMBIENT,s10_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s10_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s10_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s10_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//3
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s16
	glMaterialfv(GL_FRONT,GL_AMBIENT,s16_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s16_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s16_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s16_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//4
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s22
	glMaterialfv(GL_FRONT,GL_AMBIENT,s22_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s22_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s22_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s22_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//fifth row
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-6.0f);
	//1
	//glPushMatrix();
	glTranslatef(-0.7f,0.0f,0.0f);
	
	//s5
	glMaterialfv(GL_FRONT,GL_AMBIENT,s5_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s5_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s5_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s5_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//2
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s11
	glMaterialfv(GL_FRONT,GL_AMBIENT,s11_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s11_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s11_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s11_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//3
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s17
	glMaterialfv(GL_FRONT,GL_AMBIENT,s17_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s17_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s17_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s17_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//4
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s23
	glMaterialfv(GL_FRONT,GL_AMBIENT,s23_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s23_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s23_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s23_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//sixth row
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,-0.5f,-6.0f);
	//1
	//glPushMatrix();
	glTranslatef(-0.7f,0.0f,0.0f);
	
	//s6
	glMaterialfv(GL_FRONT,GL_AMBIENT,s6_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s6_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s6_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s6_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//2
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s12
	glMaterialfv(GL_FRONT,GL_AMBIENT,s12_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s12_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s12_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s12_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//3
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s18
	glMaterialfv(GL_FRONT,GL_AMBIENT,s18_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s18_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s18_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s18_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//4
	//glPushMatrix();
	glTranslatef(0.5f,0.0f,0.0f);
	
	//s24
	glMaterialfv(GL_FRONT,GL_AMBIENT,s24_material_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,s24_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,s24_material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,s24_material_shininess);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f,30,30);
	//glPopMatrix();
	
	//glPopMatrix();
	SwapBuffers(ghdc);
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
