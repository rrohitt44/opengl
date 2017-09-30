#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

using namespace std;

//for creating window
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;
int main(void){
	void CreateWindow(void);
	void uninitialize(void);
	
	
	CreateWindow();
	
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
			}
			break;
			case 33:
					uninitialize();
					exit(0);
				break;
		}
	}
}

void CreateWindow(void){
	void uninitialize(void);
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay==NULL){
		printf("ERROR : Unable To Open X Display. \nExitting Now...\n");
		uninitialize();
		exit(1);
	}
	defaultScreen = XDefaultScreen(gpDisplay);
	defaultDepth = DefaultDepth(gpDisplay, defaultScreen);
	
	gpXVisualInfo = (XVisualInfo *) malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo==NULL){
		printf("ERROR : Unable To Allocate Memory for Visual Info. \nExitting Now...\n");
		uninitialize();
		exit(1);
	}
	/*XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo);
	if(gpXVisualInfo==NULL){
		printf("ERROR : Unable To Get A Visual. \nExitting Now...\n");
		uninitialize();
		exit(1);
	}*/
	if(XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo)==0)
	{
		printf("ERROR : Unable To Get A Visual. \nExitting Now...\n");
		uninitialize();
		exit(1);
	}
	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				gpXVisualInfo->visual,
				AllocNone);
	gColormap = winAttribs.colormap;
	winAttribs.background_pixel = BlackPixel(gpDisplay,defaultScreen);
	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow = XCreateWindow(gpDisplay,
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
		printf("ERROR : Failed to Create Main Window. \nExitting Now...\n");
		uninitialize();
		exit(1);
	}
	XStoreName(gpDisplay,gWindow,"My first plain window without fullscreen in XWINDOWS");
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow, &windowManagerDelete,1);
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
