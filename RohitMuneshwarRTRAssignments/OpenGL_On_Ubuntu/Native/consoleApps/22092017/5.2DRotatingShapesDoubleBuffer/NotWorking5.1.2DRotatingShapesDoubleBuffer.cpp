//headers
#include<iostream>
#include<stdio.h> //for printf()
#include<stdlib.h> //for exit()
#include<memory.h> //for memset()

//headers for XServers
#include<X11/Xlib.h> //analogous to windows.h
#include<X11/Xutil.h> //for visual
#include<X11/XKBlib.h> //XkbKeycodeToKeysym()
#include<X11/keysym.h> //for KeySym

#include<GL/gl.h>
#include<GL/glx.h> //for glx functions
#include<GL/glu.h>

using namespace std;


//global variable declarations
FILE *gpFile=NULL;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth=800;
int giWindowHeight=600;

bool gbFullscreen=false;

typedef GLXContext (*glXCreateContextAttribsARBProc) (Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB=NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext; //parallel to HGLRC

//entry-point function
int main(void)
{
	//function prototypes
	void CreateWindow(void);
	void uninitialize(void);
	void ToggleFullscreen(void);
	void initialize(void);
	void resize(int,int);
	void display(void);
	
	//local variables
	int liWinWidth=giWindowWidth;
	int liWinHeight=giWindowHeight;
	
	//code
	//create log file
	gpFile=fopen("log.txt","w");
	if(gpFile==NULL)
	{
		printf("Log File Can Not Be Created.\nExiting Now...\n");
		exit(1);
	}else
	{
		fprintf(gpFile,"Log File Is Successfully Opened.\n");
	}
	
	//create the window
	CreateWindow();
	printf("Created window successfully...\n");
	//initialize
	initialize();
	printf("after initialize...\n");
	//message loop
	XEvent event;
	KeySym keysym;
	bool bDone=false;
	while(bDone==false)
	{
		while(XPending(gpDisplay))
		{
		XNextEvent(gpDisplay, &event);
		switch(event.type)
		{
			case 33:
				bDone=true;
			break;
			
			case KeyPress:
				keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
				switch(keysym)
				{
					case XK_Escape:
						bDone=true;
					break;
					case XK_F:
					case XK_f:
						if(gbFullscreen==false)
						{
							ToggleFullscreen();
							gbFullscreen=true;
						}else
						{
							ToggleFullscreen();
							gbFullscreen=false;
						}
					break;
					default:
					break;
				}
			break;
			
			case ButtonPress:
				switch(event.xbutton.button)
				{
					case 1:
					//left mouse button
					break;
					case 2:
					//middle
					break;
					case 3:
					//right
					break;
					case 4:
					//mouse wheel up
					break;
					case 5:
					//mouse wheel down
					break;
					default:
					break;
				}
			break;
			
			case MotionNotify:
			//mouse_move
			break;
			
			case ConfigureNotify:
			//wm_size
				liWinWidth=event.xconfigure.width;
				liWinHeight=event.xconfigure.height;
				resize(liWinWidth,liWinHeight);
			break;
			
			case Expose:
			//wm_paint
			break;
			
			case DestroyNotify:
			//wm_destroy
			break;
			default:
			break;
		}
	}
	
	display();
	}
	uninitialize();
	return (0);
}

void CreateWindow(void)
{
	void uninitialize(void);
	
	XSetWindowAttributes winAttribs;
	//int defaultScreen;
	//int defaultDepth;
	int styleMask;
	GLXFBConfig *pGLXFBConfig=NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo=NULL;
	int iNumFBConfigs=0;
	int i;
	
	static int frameBufferAttributes[]={
				GLX_X_RENDERABLE,True,
				GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
				GLX_RENDER_TYPE,GLX_RGBA_BIT,
				GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,
				GLX_RED_SIZE,8,
				GLX_GREEN_SIZE,8,
				GLX_BLUE_SIZE,8,
				GLX_ALPHA_SIZE,8,
				GLX_DEPTH_SIZE,24,
				GLX_STENCIL_SIZE,8,
				GLX_DOUBLEBUFFER,True,
				//GLX_SAMPLE_BUFFERS,1,
				//GLX_SAMPLES,4,
				None
	};
	gpDisplay=XOpenDisplay(NULL);
	
	if(gpDisplay==NULL)
	{
		printf("ERROR: unable to open X Display.\nExiting now...\n");
		uninitialize();
		exit(1);
	}
	
	/*defaultScreen=XDefaultScreen(gpDisplay);
	defaultDepth=DefaultDepth(gpDisplay,defaultScreen);
	
	gpXVisualInfo=(XVisualInfo *) malloc(sizeof(XVisualInfo));
	
	if(gpXVisualInfo==NULL)
	{
		printf("ERROR: unable to allocate memory for visual info.\nExiting now...\n");
		uninitialize();
		exit(1);
	}
	
	if((XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo))==0)
	{
		printf("ERROR: unable to get a visual.\nExiting now...\n");
		uninitialize();
		exit(1);
	}*/
	
	pGLXFBConfig=glXChooseFBConfig(gpDisplay,DefaultScreen(gpDisplay),frameBufferAttributes,&iNumFBConfigs);
	if(pGLXFBConfig==NULL)
	{
		printf("ERROR: unable to get a valid framebuffer config.\nExiting now...\n");
		uninitialize();
		exit(1);
	}
	printf("%d Matching FB Configs Found.\n",iNumFBConfigs);
	
	int bestFrameBufferConfig=-1,worstFrameBufferConfig=-1,bestNumberOfSamples=-1,worstNumberOfSamples=999;
	printf("Entering the loop\n");
	for(i=0;i<iNumFBConfigs;i++)
	{
		pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfig[i]);
		if(pTempXVisualInfo)
		{
			int sampleBuffer, samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfig[i],GLX_SAMPLE_BUFFERS,&sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfig[i],GLX_SAMPLES, &samples);
			printf("Matching Framebuffer Config=%d : Visual ID=0x%lu : SAMPLE_BUFFERS=%d : SAMPLES=%d\n",i,pTempXVisualInfo->visualid,sampleBuffer,samples);
			
			if(bestFrameBufferConfig<0 || sampleBuffer && samples > bestNumberOfSamples)
			{
				bestFrameBufferConfig=i;
				bestNumberOfSamples=samples;
			}
			
			if(worstFrameBufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
			{
				worstFrameBufferConfig=i;
				worstNumberOfSamples=samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	printf("Exiting the loop\n");
	
	bestGLXFBConfig=pGLXFBConfig[bestFrameBufferConfig];
	gGLXFBConfig=bestGLXFBConfig;
	printf("...bestFrameBufferConfig-%d\n",bestFrameBufferConfig);
	XFree(pGLXFBConfig);
	
	gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,bestGLXFBConfig);
	printf("Chosen Visual ID=0x%lu\n",gpXVisualInfo->visualid);
	
	winAttribs.border_pixel=0;
	//winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,
					RootWindow(gpDisplay,gpXVisualInfo->screen),
					gpXVisualInfo->visual,
					AllocNone);
	gColormap=winAttribs.colormap;
	winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask |
				KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow=XCreateWindow(gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,
				0,
				giWindowWidth,
				giWindowHeight,
				0,
				gpXVisualInfo->depth,
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs);
				
	if(!gWindow)
	{
		printf("ERROR: unable to create main window.\nExiting now...\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay,gWindow,"OpenGL DB- 2D Rotating Shapes");
	
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	XMapWindow(gpDisplay,gWindow);
}

void uninitialize(void)
{
	GLXContext currentContext=glXGetCurrentContext();
	if(currentContext!=NULL && currentContext==gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
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
		fprintf(gpFile,"Log File Is Closed Successfully.\n");
		fclose(gpFile);
		gpFile=NULL;
	}
}

void ToggleFullscreen(void)
{
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};
	
	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));
	
	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=gbFullscreen ? 0 : 1;
	
	fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1]=fullscreen;
	
	XSendEvent(gpDisplay,
		RootWindow(gpDisplay,gpXVisualInfo->screen),
		False,
		StructureNotifyMask,
		&xev);
}

void initialize(void)
{
	void uninitialize(void);
	void resize(int,int);
	printf("entered initialize...\n");
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB((GLubyte *) "glXCreateContextAttribsARB");
	printf("after creating contextAttribs...\n");
	/*GLint attribs[]={
			GLX_CONTEXT_MAJOR_VERSION_ARB,4,
			GLX_CONTEXT_MINOR_VERSION_ARB,5,
			GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
	};*/
	GLint attribs[]={
			GLX_CONTEXT_MAJOR_VERSION_ARB,3,
			GLX_CONTEXT_MINOR_VERSION_ARB,1,
			GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
	};
	
	
	gGLXContext=glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	printf("after creating context attribs arb...\n");
	if(!gGLXContext)
	{
		GLint attribs[]={
			GLX_CONTEXT_MAJOR_VERSION_ARB,1,
			GLX_CONTEXT_MINOR_VERSION_ARB,0,
			//GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
		};
		printf("Failed to create GLX 4.5 context. Hence using old style GLX context.\n");
		gGLXContext=glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	}else
	{
		printf("OpenGL Context 3.1 Is Created.\n");
	}
	
	if(!glXIsDirect(gpDisplay,gGLXContext))
	{
		printf("Indirect GLX Rendering context obtained.\n");
	}else
	{
		printf("Direct GLX Rendering context obtained.\n");
	}
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	resize(giWindowWidth,giWindowHeight);
}

void resize(int width,int height)
{
	if(height==0)
	{
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0f,50.0f,-50.0f,50.0f,-50.0f,50.0f);*/
}

void display(void)
{
	//function prototypes
	void drawTriangle(void);
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-6.0f);*/
	//drawTriangle();
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glEnd();
	glXSwapBuffers(gpDisplay,gWindow);
}

void drawTriangle(void)
{
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(0.0f,0.5f,0.0f);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(-0.5f,-0.5f,0.0f);
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(0.5f,-0.5f,0.0f);
	glEnd();
}
