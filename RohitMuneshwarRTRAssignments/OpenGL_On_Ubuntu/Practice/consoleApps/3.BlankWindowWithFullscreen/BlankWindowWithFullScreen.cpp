 //header files
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

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

//entry-point function
int main(void)
{
	//function prototypes
	void createWindow(void);
	void uninitialize(void);
	void toggleFullscreen(void);
	
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
	
	//Message Loop
	XEvent event;
	KeySym keysym;
	
	while(1)
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
						uninitialize();
						exit(0);
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
				break;
			case Expose:
				//WM_PAINT
				break;
			case DestroyNotify:
				//WM_DESTROY
				break;
			case 33:
				uninitialize();
				exit(0);
				break;
			default:
				break;
		}
		
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
	int defaultDepth;
	int styleMask;
	
	//code
	gpDisplay = XOpenDisplay(NULL); //ususally pass the argv to open connection with XServer
	if(gpDisplay==NULL)
	{
		printf("ERROR: Unable To Open X Display.\nExiting...");
		uninitialize();
		exit(1);
	}
	
	defaultScreen = XDefaultScreen(gpDisplay);
	defaultDepth = DefaultDepth(gpDisplay,defaultScreen);
	
	gpXVisualInfo = (XVisualInfo *) malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo==NULL)
	{
		printf("ERROR: Unable To Allocate Memory For Visual Info.\nExitting Now...\n");
		uninitialize();
		exit(1);
	}
	
	XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo);
	if(gpXVisualInfo==NULL)
	{
		printf("ERROR: Unable To Get A Visual.\nExitting Now...\n");
		uninitialize();
		exit(1);
	}
	
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
				
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow = XCreateWindow( gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,0,
				giWinWidth,giWinHeight,
				0,
				gpXVisualInfo->depth,
				InputOutput,
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
	
	XStoreName(gpDisplay,gWindow,"First Window - Escape and Fullscreen");
	Atom windowManagerDelete = XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
}

void uninitialize(void)
{
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
	
	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),False,StructureNotifyMask,&xev);
}
