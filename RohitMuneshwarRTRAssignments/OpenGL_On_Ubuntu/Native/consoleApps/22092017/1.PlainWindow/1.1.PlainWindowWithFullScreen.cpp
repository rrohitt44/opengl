#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

using namespace std;

Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;

bool gbFullscreen = false;

int main(void)
{
	void CreateWindow(void);
	void uninitialize(void);
	void ToggleFullscreen(void);
	
	CreateWindow();
	
	//message loop
	XEvent event;
	KeySym keysym;
	while(1)
	{
		XNextEvent(gpDisplay,&event);
		switch(event.type)
		{
			case MapNotify:
			//like WM_CREATE
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
					if(gbFullscreen==false)
					{
						ToggleFullscreen();
						gbFullscreen = true;
					}else
					{
						ToggleFullscreen();
						gbFullscreen = false;
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
					default:
					break;
				}
			break;
			case MotionNotify:
			//mouse move
			break;
			case ConfigureNotify:
			//WM_SIZE
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
}

void CreateWindow(void)
{
	void uninitialize(void);
	
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;
	
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR: Unable to open XDisplay.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}
	defaultScreen=XDefaultScreen(gpDisplay);
	defaultDepth=DefaultDepth(gpDisplay,defaultScreen);
	
	gpXVisualInfo=(XVisualInfo *)malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo==NULL)
	{
		printf("ERROR: Unable to allocate memroy.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}
	if(XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo)==0)
	{
		printf("ERROR: Unable to get a visual.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}
	
	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
				gpXVisualInfo->visual,
				AllocNone);
	gColormap = winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | 
				ButtonPressMask | KeyPressMask | PointerMotionMask
				| StructureNotifyMask;
				
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow = XCreateWindow(gpDisplay,
				RootWindow(gpDisplay, gpXVisualInfo->screen),
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
		printf("ERROR: Failed to create main window.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}
	XStoreName(gpDisplay,gWindow,"First pain window with fullscreen in XWINDOWS");
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
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
}

void ToggleFullscreen(void)
{
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};
	
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE",False);
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
