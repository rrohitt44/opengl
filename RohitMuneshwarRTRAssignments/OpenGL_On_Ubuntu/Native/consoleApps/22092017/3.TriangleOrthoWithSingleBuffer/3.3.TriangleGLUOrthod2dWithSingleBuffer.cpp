#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

//namespace
using namespace std;

//global variable declarations
FILE *gpFile=NULL;
int giWinWidth=800;
int giWinHeight=600;
Display *gpDisplay=NULL;
bool gbFullScreen=false;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
GLXContext gGLXContext;
//entry point function
int main(void)
{
	//function prototypes
	void createWindow(void);
	void initialize(void);
	void toggleFullScreen(void);
	void resize(int,int);
	void display(void);
	
	//variable declaration
	int winWidth=giWinWidth;
	int winHeight=giWinHeight;
	bool bDone=false;
	
	//code
	//create log file
	gpFile=fopen("log.txt","w");
	if(gpFile==NULL)
	{
		printf("ERROR: Log File Cannot Be Created.\nExitting Now...\n");
		exit(1);
	}else
	{
		fprintf(gpFile,"Log File Is Succefully Opened...\n");
	}
	
	//creates the window
	createWindow();
	
	//initializes the window
	initialize();
	
	//message loop
	XEvent event;
	KeySym keysym;
	
	while(bDone==false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay,&event);
			switch(event.type)
			{
				case 33:
					bDone=true;
				break;
				
				//WM_KEYDOWN
				case KeyPress:
					keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keysym)
					{
						case XK_Escape:
							bDone=true;
						break;
						case XK_f:
						case XK_F:
							if(gbFullScreen==false)
							{
								toggleFullScreen();
								gbFullScreen=true;
							}else
							{
								toggleFullScreen();
								gbFullScreen=false;
							}
						break;
						default:
						break;
					}
				break;
				
				//WM_SIZE
				case ConfigureNotify:
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
					
				//other events
				case MapNotify:
					//WM_CREATE
					break;
					
				case MotionNotify:
					//WM_MOUSEMOVE
					break;
					
				case Expose:
					//WM_PAINT
					break;
					
				case DestroyNotify:
					//WM_DESTROY
					break;
				default:
				break;
			}
		}
		display();
	}
	return 0;
}

void createWindow(void)
{
	//function prototypes
	void uninitialize(void);
	
	//variable declarations
	int defaultScreen;
	int styleMask;
	static int frameBufferAttributes[]={
					GLX_RGBA,
					GLX_RED_SIZE,1,
					GLX_GREEN_SIZE,1,
					GLX_BLUE_SIZE,1,
					GLX_ALPHA_SIZE,1,
					None	
					};
	XSetWindowAttributes winAttribs;
	
	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR: Unable To Open A Display.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}
	
	defaultScreen=XDefaultScreen(gpDisplay);
	gpXVisualInfo=glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);
	
	winAttribs.border_pixel=0;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,
					RootWindow(gpDisplay,gpXVisualInfo->screen),
					gpXVisualInfo->visual,
					AllocNone);
	gColormap=winAttribs.colormap;
	winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	
	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow=XCreateWindow(
				gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,
				0,
				giWinWidth,
				giWinHeight,
				0,
				gpXVisualInfo->depth,
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs
			);
	if(!gWindow)
	{
		printf("ERROR: Failed To Create A Window in Memory.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay,gWindow,"OpenGL SB: Triangle using GluOrtho2D Projection Perspective");
	
	Atom winManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&winManagerDelete,1);
	XMapWindow(gpDisplay,gWindow);
}

void initialize(void)
{
	//function prototypes
	void resize(int,int);
	
	//code
	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	glClearColor(0.0f,0.0f,1.0f,0.0f);
	
	resize(giWinWidth,giWinHeight);
}

void resize(int width,int height)
{
	//code
	if(height==0)
	{
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0f,1.0f,-1.0f,0.0f);
}

void display(void)
{
	//function prototypes
	void drawTriangle(void);
	
	//code
	glClear(GL_COLOR_BUFFER_BIT);
	
	drawTriangle();
	
	glFlush();	
}

void drawTriangle(void)
{
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,0.0f);
	glColor3f(0.0f,1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,0.0f);
	glEnd();
}

void uninitialize(void)
{
	//variable declaration
	GLXContext currentGLXContext;
	currentGLXContext=glXGetCurrentContext();
	
	if(currentGLXContext!=NULL && currentGLXContext==gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
	}	
	
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay,gGLXContext);
	}
	
	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}
	
	if(gColormap)
	{
		XFreeColormap(gpDisplay,gColormap);
	}
	
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
	
	if(gpFile)
	{
		fprintf(gpFile,"Log File Is Successfully Closed...\n");
		fclose(gpFile);
		gpFile=NULL;
	}
}

void toggleFullScreen(void)
{
	//variable declaration
	Atom wm_state;
	XEvent xev={0};
	Atom fullScreen;
	
	//code
	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));
	
	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=gbFullScreen ? 0 : 1;
	
	fullScreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1]=fullScreen;
	
	XSendEvent(
		gpDisplay,
		RootWindow(gpDisplay,gpXVisualInfo->screen),
		False,
		StructureNotifyMask,
		&xev	
		);
}
