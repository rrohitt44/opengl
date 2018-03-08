#include<windows.h>
#include<stdio.h> //for file IO
#include<gl/glew.h> //for GLSL extensions
#include<gl/GL.h>

#include "vmath.h"

#pragma comment(lib,"glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

enum
{
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

bool gbDone = false;
bool gbFullScreen = false;
HWND ghwnd=NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)}; //for fullscreen

//changes for opengl
bool gbActiveWindow = false;
bool gbEscapePressed = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

//log file
FILE *g_fp_logfile = NULL;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_pyramid;
GLuint gVbo_pyramid_position;
GLuint gVbo_pyramid_normal;

GLuint gModelMatrixUniform, gViewMatrixUniform, gProjectionMatrixUniform;

GLuint gL0aUniform;
GLuint gL0dUniform;
GLuint gL0sUniform;
GLuint gL0pUniform;
 
GLuint gL1aUniform;
GLuint gL1dUniform;
GLuint gL1sUniform;
GLuint gL1pUniform;

GLuint gKaUniform;
GLuint gKdUniform;
GLuint gKsUniform;
GLuint gKShininessUniform;

GLuint gLKeyPressedUniform;


mat4 gPerspectiveProjectionMatrix;

bool gbLight;

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
GLfloat material_shininess=50.0f;

GLfloat gfAnglePyra = 0.0f;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLIne, int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void updateAngle(void);
	WNDCLASSEX wndclass;
	TCHAR AppName[] = TEXT("Window Custom");
	TCHAR WinName[] = TEXT("PP Lights - Pyramid - Per Vertex Phong Model");
	HWND hwnd;
	MSG msg;
	RECT rect;
	
	if(fopen_s(&g_fp_logfile,"ppLog.txt","w")!=0)
	{
		MessageBox(NULL,TEXT("Log File Can Not Be Created\nExitting..."),TEXT("Error"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
		uninitialize();
	}else
	{
		fprintf(g_fp_logfile,"\nLog file is opened successfully...\n");
	}
	
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
			updateAngle();
			if(gbActiveWindow == true){
				if(gbEscapePressed == true){
					gbDone = true;
				}
				
				display(); //for double buffer
			}
		}
	}
	
	uninitialize();
	return ((int) msg.wParam);
}

void updateAngle(void)
{
	if(gfAnglePyra>=360)
	{
		gfAnglePyra = gfAnglePyra - 360;
	}
	gfAnglePyra = gfAnglePyra + 0.01f;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
	void ToggleFullscreen(void);
	void resize(int, int);
	//void display(void);
	void uninitialize(void);
	
	
	
	//variable declarations
	static bool bIsAKeyPressed = false;
	static bool bIsLKeyPressed = false;
	
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
					if(gbEscapePressed==false)
						gbEscapePressed = true;
					break;
				break;
				case 0x4C: // for 'L' or 'l'
					if (bIsLKeyPressed == false)
					{
						gbLight = true;
						bIsLKeyPressed = true;
					}
					else
					{
						gbLight = false;
						bIsLKeyPressed = false;
					}
					break;
				case 0x46: //F or f key
					if(gbFullScreen==false)
					{
						ToggleFullscreen();
						gbFullScreen = true;
					}else
					{
						ToggleFullscreen();
						gbFullScreen = false;
					}
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
	void uninitialize(void);
	
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	
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
	pfd.cDepthBits=32;
	
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
	
	//GLEW initialization code for GLSL
	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	
	//VERTEX SHADER
	//create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	
	//provide source code to shader
	const GLchar *vertexShaderSourceCode =
				"#version 440							"\
				"\n										"\
				"in vec4 vPosition;						"\
				"in vec3 vNormal;"\
				"uniform mat4 u_model_matrix;"\
				"uniform mat4 u_view_matrix;"\
				"uniform mat4 u_projection_matrix;"\
				"uniform int u_LKeyPressed;"\
				"uniform vec3 u_L0a;"\
				"uniform vec3 u_L0d;"\
				"uniform vec3 u_L0s;"\
				"uniform vec4 u_L0p;"\
				"uniform vec3 u_L1a;"\
				"uniform vec3 u_L1d;"\
				"uniform vec3 u_L1s;"\
				"uniform vec4 u_L1p;"\
				"uniform vec3 u_Ka;"\
				"uniform vec3 u_Kd;"\
				"uniform vec3 u_Ks;"\
				"uniform float u_KShininess;"\
				"out vec3 phong_ads_color;"\
				"void main(void)						"\
				"{										"\ 
				"if(u_LKeyPressed == 1)"\
				"{"\
				"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"\
				"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"\
				"vec3 light0_direction = normalize(vec3(u_L0p) - eyeCoordinates.xyz);"\
				"float tn_dot_ld0=max(dot(transformed_normals, light0_direction),0.0);"\
				"vec3 ambient0 = u_L0a * u_Ka;"\
				"vec3 diffuse0 = u_L0d * u_Kd * tn_dot_ld0;"\
				"vec3 reflection_vector0 = reflect(-light0_direction, transformed_normals);"\
				"vec3 viewer_vector0 = normalize(-eyeCoordinates.xyz);"\
				"vec3 specular0 = u_L0s * u_Ks * pow(max(dot(reflection_vector0, viewer_vector0),0.0), u_KShininess);"\
				"vec3 light_direction1 = normalize(vec3(u_L1p) - eyeCoordinates.xyz);"\
				"float tn_dot_ld1=max(dot(transformed_normals, light_direction1),0.0);"\
				"vec3 ambient1 = u_L1a * u_Ka;"\
				"vec3 diffuse1 = u_L1d * u_Kd * tn_dot_ld1;"\
				"vec3 reflection_vector1 = reflect(-light_direction1, transformed_normals);"\
				"vec3 viewer_vector1 = normalize(-eyeCoordinates.xyz);"\
				"vec3 specular1 = u_L1s * u_Ks * pow(max(dot(reflection_vector1, viewer_vector1),0.0), u_KShininess);"\
				"phong_ads_color=(ambient0+ambient1)+(diffuse0+diffuse1)+(specular0+specular1);"\
				"}"\
				"else"\
				"{"\
				"phong_ads_color=vec3(1.0,1.0,1.0);"\
				"}"\
				"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;	"\
				"}										";
				
				
	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode,NULL);
	fprintf(g_fp_logfile,"Actual shadig language version is %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
	//compile shader
	glCompileShader(gVertexShaderObject);
	
	GLint iInfoLogLength=0;
	GLint iShaderCompileStatus=0;
	char *szInfoLog=NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if(iShaderCompileStatus==GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(g_fp_logfile, "vertex shader compilation log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	
	//FRAGMENT SHADER
	//create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	
	//provide source code to shader
	const GLchar *fragmentShaderSourceCode =
				"#version 440" \
				"\n" \
				"in vec3 phong_ads_color;"\
				"out vec4 FragColor;" \
				"void main(void)" \
				"{" \
				"FragColor=vec4(phong_ads_color, 1.0);" \
				"}";
				
	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode,NULL);
	
	//compile shader
	glCompileShader(gFragmentShaderObject);
	//reinitialize 
	 iInfoLogLength = 0;
	 iShaderCompileStatus = 0;
	 szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if(iShaderCompileStatus==GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength>0)
		{
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(g_fp_logfile,"Fragment Shader Compilation Log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	
	//SHADER PROGRAM
	//create
	gShaderProgramObject = glCreateProgram();
	
	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObject,gVertexShaderObject);

	//attach fragment shader to shader program
	glAttachShader(gShaderProgramObject,gFragmentShaderObject);
	
	//pre-link binding of shader program object with vertex shader position attribute
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX,"vPosition");
	
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_NORMAL,"vNormal");
	
	//link shader
	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus=0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if(iShaderProgramLinkStatus==GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength>0)
		{
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(g_fp_logfile, "Shader Program Link Log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	
	//get MVP uniform location
	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	
	gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	
	gL0aUniform = glGetUniformLocation(gShaderProgramObject, "u_L0a");
	gL0dUniform = glGetUniformLocation(gShaderProgramObject, "u_L0d");
	gL0sUniform = glGetUniformLocation(gShaderProgramObject, "u_L0s");
	gL0pUniform = glGetUniformLocation(gShaderProgramObject, "u_L0p");
	
	gL1aUniform = glGetUniformLocation(gShaderProgramObject, "u_L1a");
	gL1dUniform = glGetUniformLocation(gShaderProgramObject, "u_L1d");
	gL1sUniform = glGetUniformLocation(gShaderProgramObject, "u_L1s");
	gL1pUniform = glGetUniformLocation(gShaderProgramObject, "u_L1p");
	
	gKaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	gKdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	gKsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	gKShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_KShininess");
	
	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	//pyramid draw starts here
	const GLfloat pyramidVertices[] =
	{
		0, 1, 0,
		-1, -1, 1,
		1, -1, 1,

		0, 1, 0,
		1, -1, 1,
		1, -1, -1,

		0, 1, 0,
		1, -1, -1,
		-1, -1, -1,

		0, 1, 0,
		-1, -1, -1,
		-1, -1, 1
	};
	
	const GLfloat pyramidNoramals[] =
	{
		0.0f,0.447214f,0.894427f,
		0.0f,0.447214f,0.894427f,
		0.0f,0.447214f,0.894427f,
		
		0.894427f,0.447214f,0.0f,
		0.894427f,0.447214f,0.0f,
		0.894427f,0.447214f,0.0f,
		
		0.0f,0.447214f,-0.0894427f,
		0.0f,0.447214f,-0.0894427f,
		0.0f,0.447214f,-0.0894427f,
		
		-0.894427f,0.447214f,0.0f,
		-0.894427f,0.447214f,0.0f,
		-0.894427f,0.447214f,0.0f
	};
	
	// vao
    glGenVertexArrays(1, &gVao_pyramid);
    glBindVertexArray(gVao_pyramid);

    // position vbo
    glGenBuffers(1, &gVbo_pyramid_position);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // normal vbo
    glGenBuffers(1, &gVbo_pyramid_normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNoramals), pyramidNoramals, GL_STATIC_DRAW);

    glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // unbind vao
    glBindVertexArray(0);
	
	glShadeModel(GL_SMOOTH);
	
	//set-up depth buffer
	glClearDepth(1.0f);
	
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	
	//depth test to do
	glDepthFunc(GL_LEQUAL);
	
	//set really ice perspective calculations ?
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	//we will always cull back faces for better performance
	glEnable(GL_CULL_FACE);
	
	glClearColor(0.5f,0.5f,0.5f,0.0f);
	
	//set orthographicMatrix to identify matrix
	gPerspectiveProjectionMatrix = mat4::identity();
	
	gbLight = false;
	//resize
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // | GL_STENCIL_BUFFER add kelyawar output disat nhi.... why?
	
	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);
	
	if(gbLight==true)
	{
		glUniform1i(gLKeyPressedUniform, 1);
		glUniform3fv(gL0aUniform, 1, light0_ambient);
		glUniform3fv(gL0dUniform, 1, light0_defused);
		glUniform3fv(gL0sUniform, 1, light0_specular);
		glUniform3fv(gL0pUniform, 1, light0_position);
		
		glUniform3fv(gL1aUniform, 1, light1_ambient);
		glUniform3fv(gL1dUniform, 1, light1_defused);
		glUniform3fv(gL1sUniform, 1, light1_specular);
		glUniform3fv(gL1pUniform, 1, light1_position);
		
		glUniform3fv(gKaUniform, 1, material_ambient);
		glUniform3fv(gKdUniform, 1, material_defused);
		glUniform3fv(gKsUniform, 1, material_specular);
		glUniform1f(gKShininessUniform, material_shininess);
	}else
	{
		glUniform1i(gLKeyPressedUniform,0);
	}
	
	//OpenGL drawing
	//set modelview & modelviewprojection matrices to identity
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 rotationMatrix=mat4::identity();
	//translate
	modelMatrix = translate(0.0f,0.0f,-6.0f);
	rotationMatrix = vmath::rotate(gfAnglePyra,0.0f,1.0f,0.0f);
	modelMatrix = modelMatrix * rotationMatrix;
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_pyramid);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLES, 0,12);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	//stop using OpenGL program object
	glUseProgram(0);
	
	SwapBuffers(ghdc);
}

void resize(int width,int height){
	if(height == 0){
		height = 1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	
	//glOrtho(left,right,bottom,top,near,far);
	/*if(width<=height)
	{
			gOrthographicProjectionMatrix = ortho(-100.0f,100.0f, (-100.0f * (height/width)), (100.0f * (height/width)), -100.0f, 100.0f);
	}else
	{
		gOrthographicProjectionMatrix = ortho(-100.0f, 100.0f, (-100.0f * (width/height)), (100.0f * (width/height)), -100.0f, 100.0f);
	}*/
	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f,100.0f);
}


void uninitialize(){
	if(gbFullScreen == true){
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
	
	//destroy vao
	if(gVao_pyramid)
	{
		glDeleteVertexArrays(1, &gVao_pyramid);
		gVao_pyramid = 0;
	}
	
	//destroy vbo
	if(gVbo_pyramid_position)
	{
		glDeleteBuffers(1, &gVbo_pyramid_position);
		gVbo_pyramid_position = 0;
	}
	
	if(gVbo_pyramid_normal)
	{
		glDeleteBuffers(1, &gVbo_pyramid_normal);
		gVbo_pyramid_normal = 0;
	}
	
	
	//detach vertex shader from shader program object
	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	
	//detach fragment shader from shader program object
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);
	
	//delete vertex shader object
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	
	//delete fragment shader object
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject=0;
	
	//delete shader program object
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject=0;
	
	//unlink shader program
	glUseProgram(0);
	
	//deselect the rendering context
	wglMakeCurrent(NULL,NULL);
	
	//delete the rendering context
	wglDeleteContext(ghrc);
	ghrc = NULL;
	
	//delete the device context
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;
	
	if(g_fp_logfile){
		fprintf(g_fp_logfile,"\nLog file is closed successfully...\n");
		fclose(g_fp_logfile);
		g_fp_logfile = NULL;
	}
	DestroyWindow(ghwnd);
	ghwnd = NULL;
}