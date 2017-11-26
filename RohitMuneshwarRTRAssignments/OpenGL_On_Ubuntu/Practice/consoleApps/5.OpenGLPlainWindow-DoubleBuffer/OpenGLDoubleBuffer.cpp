 //header files
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


//namespace
using namespace std;

//global variable declarations
int giWinWidth = 800;
int giWinHeight = 600;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
bool bFullscreen = false;
int giXWinPos=100;
int giYWinPos=100;
FILE *gpFile=NULL;

GLXContext gGLXContext; //parallel to HGLRC
typedef GLXContext (*glXCreateContextAttribsARBProc) (Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB=NULL;
GLXFBConfig gGLXFBConfig;
//entry-point function
int main(void)
{
	//function prototypes
	void createWindow(void);
	void uninitialize(void);
	void toggleFullscreen(void);
	void initialize(void);
	void resize(GLint,GLint);
	void display(void);
	
	//local varialbles
	int winWidth = giWinWidth;
	int winHeight = giWinHeight;
	
	//code
	gpFile=fopen("Log.txt","w");
	if(gpFile==NULL)
	{
		printf("Log File Cannot Be Created...\nExitting Now...\n");
		exit(0);
	}else
	{
		fprintf(gpFile,"Log File Successfully Created.\n");
	}
	
	createWindow();
	
	initialize();
	
	//Message Loop
	XEvent event;
	KeySym keysym;
	GLboolean bDone=false;
	while(bDone==false)
	{
		while(XPending(gpDisplay))
		{
		
			XNextEvent(gpDisplay,&event);
			switch(event.type)
			{
				case MapNotify:
					//WM_CREATE
					break;
				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keysym)
					{	
						case XK_Escape:
							bDone=true;
							break;
						case XK_F:
						case XK_f:
							if(bFullscreen == false)
							{
								toggleFullscreen();
								bFullscreen = true;
							}else
							{
								toggleFullscreen();
								bFullscreen = false;
							}
							break;
						default:
							break;
					}
					break;
				case ButtonPress:
					//mouse button press
					switch(event.xbutton.button)
					{
						case 1:
							//left mouse button
							break;
						case 2:
							//middle mouse button
							break;
						case 3:
							//right mouse button
							break;
						case 4:
							//mouse wheel up
							break;
						case 5:
							//mouse wheel down
							break;
					}
					break;
				case MotionNotify:
					//MOUSE_MOVE
					break;
				case ConfigureNotify:
					//WM_SIZE
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
				case Expose:
					//WM_PAINT
					break;
				case DestroyNotify:
					//WM_DESTROY
					break;
				case 33:
					bDone=true;
					break;
				default:
					break;
			}
		}
		display();
	}
	uninitialize();
		return 0;
}

void createWindow(void)
{
	//function prototypes
	void uninitialize(void);
	
	//variable declarations
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	
	int styleMask;
	
	GLXFBConfig *pGLXFBConfigs=NULL;
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
					None //array must be terminated by 0
					};	
	//code
	gpDisplay = XOpenDisplay(NULL); //ususally pass the argv to open connection with XServer
	if(gpDisplay==NULL)
	{
		printf("ERROR: Unable To Open X Display.\nExiting...");
		uninitialize();
		exit(1);
	}
	
	defaultScreen = XDefaultScreen(gpDisplay);
	
	//get a new framebuffer config that meets our attrib requirements
	pGLXFBConfigs=glXChooseFBConfig(gpDisplay,defaultScreen,frameBufferAttributes,&iNumFBConfigs);
	if(pGLXFBConfigs==NULL)
	{
		fprintf(gpFile,"Failed To Get Valid Framebuffer Config.\nExitting Now...\n");
		uninitialize();
		exit(1);
	}
	fprintf(gpFile,"%d Matching FB Configs Found.\n",iNumFBConfigs);
	
	//pick that FB config/visual with the most samples per pixel
	int bestFrameBufferConfig=-1,worstFrameBufferConfig=-1,bestNumberOfSamples=-1,worstNumberOfSamples=999;
	
	for(i=0;i<iNumFBConfigs;i++)
	{
		pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfigs[i]);
		if(pTempXVisualInfo)
		{
			int sampleBuffer,samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLE_BUFFERS,&sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLES,&samples);
			fprintf(gpFile,"Matching FrameBuffer Config=%d : Visual ID=%0x%lu: SAMPLE_BUFFERS=%d : SAMPLES=%d\n",i,pTempXVisualInfo->visualid,sampleBuffer,samples);
			if(bestFrameBufferConfig<0 || sampleBuffer && samples>bestNumberOfSamples)
			{
				bestFrameBufferConfig=i;
				bestNumberOfSamples=samples;
			}
			if(worstFrameBufferConfig<0 || !sampleBuffer || samples<worstNumberOfSamples)
			{
				worstFrameBufferConfig=i;
				worstNumberOfSamples=samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	
	bestGLXFBConfig=pGLXFBConfigs[bestFrameBufferConfig];
	
	//set global GLXFBConfig
	gGLXFBConfig=bestGLXFBConfig;
	
	//be sure to free FBConfig list allocated by glXChooseFBConfig()
	XFree(pGLXFBConfigs);
	
	//gpXVisualInfo = glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);
	gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,bestGLXFBConfig);
	fprintf(gpFile,"Choosen Visual ID=0x%lu\n",gpXVisualInfo->visualid);
	
	//setting window's attributes
	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay,
						RootWindow(gpDisplay,gpXVisualInfo->screen),
						gpXVisualInfo->visual,
						AllocNone);
	gColormap = winAttribs.colormap;
	
	winAttribs.background_pixel = BlackPixel(gpDisplay,defaultScreen);
	winAttribs.event_mask = ExposureMask //Expose - WM_PAINT
				| VisibilityChangeMask //MapNotify - WM_CREATE
				| ButtonPressMask //ButtonPress
				| KeyPressMask //KeyPress
				| PointerMotionMask //MotionNotify - MOUSE_MOVE
				| StructureNotifyMask //ConfigureNotify - WM_SIZE
				;
				
	styleMask = CWBorderPixel | CWEventMask | CWColormap;
	
	gWindow = XCreateWindow( gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,0,
				giWinWidth,giWinHeight,
				0, //border width
				gpXVisualInfo->depth, //depth of visual (depth for colormap)
				InputOutput,//class(type) of your window
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs
			);
			
	if(!gWindow)
	{
		printf("ERROR: Failed To Create Main Window.\nExitting now...\n");
		uninitialize();
		exit(1);	
	}
	
	XStoreName(gpDisplay,gWindow,"First Window - OpenGL DoubleBuffer");
	Atom windowManagerDelete = XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
}

void uninitialize(void)
{
	
	//variable declarations
	GLXContext currentGLXContext;
	
	//code
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
		fprintf(gpFile,"Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void toggleFullscreen(void)
{
	//variable declaration
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};
	
	//code
	wm_state = XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));
	
	xev.type=ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = bFullscreen ? 0 : 1;
	
	fullscreen = XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1] = fullscreen;
	
	//parallel to SendMessage()
	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),False,StructureNotifyMask,&xev);
}

void initialize(void)
{
	//function prototypes
	void resize(GLint,GLint);
	void uninitialize(void);
	
	//code
	//create a new GL context 5.4 for rendering
	glXCreateContextAttribsARB=(glXCreateContextAttribsARBProc) glXGetProcAddressARB((GLubyte *) "glXCreateContextAttribsARB");
	GLint attribs[]={
			GLX_CONTEXT_MAJOR_VERSION_ARB,4,
			GLX_CONTEXT_MINOR_VERSION_ARB,5,
			GLX_CONTEXT_PROFILE_MASK_ARB,
			GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0 //array must be terminated by 0
			};
	
	//gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	gGLXContext=glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	
	//fallback to safe old style 2.x context
	if(!gGLXContext)
	{
		//when a context version below 3.0 is requested, implementations will return
		//the newest context version compatible with OpenGL version less that version 3.0
		GLint attribs[] = {
					GLX_CONTEXT_MAJOR_VERSION_ARB,1,
					GLX_CONTEXT_MINOR_VERSION_ARB,0,
					0	//array must be terminated by 0
				};
		fprintf(gpFile,"Failed to create glx 5.4 context. Hence using old-style glx context.\n");
		gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	}else
	{
		fprintf(gpFile,"OpenGL Context 4.5 is created.\n");
	}
	
	//verifying that the context is a direct context
	if(!glXIsDirect(gpDisplay,gGLXContext))
	{
		fprintf(gpFile,"Indirect GLX Rendering context obtained.\n");
	}else
	{
		fprintf(gpFile,"Direct GLX rendering context obtained.\n");
	}
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	glShadeModel(GL_SMOOTH);
	
	//set-up depth buffer
	glClearDepth(1.0f);
	
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	
	//depth test to do
	glDepthFunc(GL_LEQUAL);
	
	//set really nice perspective calculations?
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	
	//we will always cull back faces for better performance
	glEnable(GL_CULL_FACE);
	
	//set background clearing color
	glClearColor(0.0f,0.0f,1.0f,0.0f);
	
	resize(giWinWidth,giWinHeight);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glXSwapBuffers(gpDisplay,gWindow);
}

void resize(GLint width, GLint height)
{
	if(height==0)
	{
		height=1;
	}
	glViewport(0,0,(GLsizei) width,(GLsizei) height);
}
