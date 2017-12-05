#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <SOIL/SOIL.h>

using namespace std;

// global variables
FILE *gpFile = NULL;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;

bool gbFullScreen = false;

int giWindowWidht = 800;
int giWindowHeight = 600;

GLXContext gGLXContext;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig,
		GLXContext, Bool, const int*);

glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;

GLXFBConfig gFBConfig;

float rotateTriangleAngle = 0.0f;
float rotateSquareAngle = 0.0f;

GLuint Texture_Smiley;

int gNumKeyPressed = 0;

int main(void) {

	//prototype
	void createWindow(void);
	void toggleFullScreen(void);
	void initialize(void);
	void display(void);
	void resize(int, int);
	void uninitialize(void);
	void rotate(void);
	//local variables
	int winWidth = giWindowWidht;
	int winHeight = giWindowHeight;
	bool bDone = false;

	//start
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL) {
		printf("ERROR: Log file can't be created.Exiting now\n\n.");
		exit(1);
	} else {
		fprintf(gpFile, "Log File opened successfully.\n\n");
	}

	createWindow();

	initialize();

	//Heart
	XEvent xEvent;
	KeySym keySym;

	while (bDone == false) {

		while (XPending(gpDisplay)) {

			XNextEvent(gpDisplay, &xEvent);

			switch (xEvent.type) {

			case MapNotify:
				break;

			case KeyPress:

				keySym = XkbKeycodeToKeysym(gpDisplay, xEvent.xkey.keycode, 0,
						0);

				switch (keySym) {

				case XK_Escape:

					uninitialize();

					exit(0);

					break;
				case XK_F:
				case XK_f:
					toggleFullScreen();
					break;
				case XK_1:
					gNumKeyPressed = 1;
					break;
				case XK_2:
					gNumKeyPressed = 2;
					break;
				case XK_3:
					gNumKeyPressed = 3;
					break;
				case XK_4:
					gNumKeyPressed = 4;
					break;
				default:
					break;
				}
				break;
			case ButtonPress:
				switch (xEvent.xbutton.button) {
				case 1:
					break;
				default:
					break;
				}
				break;
			case MotionNotify:

				break;
			case ConfigureNotify:
				winWidth = xEvent.xconfigure.width;
				winHeight = xEvent.xconfigure.height;
				resize(winWidth, winHeight);
				break;
			case DestroyNotify:
				break;
			case 33:
				bDone = true;
				break;
			default:
				break;
			}
		}
		display();
		rotate();
	}

	uninitialize();
	return 0;
}

void createWindow() {
	fprintf(gpFile, "Entered in CreateWindow()\n");
	//prototype
	void uninitialize(void);

	//local variables

	XSetWindowAttributes winAttributes;
	int defaultScreen;
	//int defaultDepth;
	int styleMask;

	GLXFBConfig *pGLXFBConfigs = NULL;
	GLXFBConfig bestGLXFBConfig;
	int iNumFBConfigs = 0;
	XVisualInfo *pTempXVisualInfo = NULL;
	int i;

	static int frameBufferAttributes[] = {
	GLX_X_RENDERABLE, True,
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE, GLX_RGBA_BIT,
	GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DEPTH_SIZE, 24,
	GLX_STENCIL_SIZE, 8,
	GLX_DOUBLEBUFFER, True,
	//GLX_SAMPLE_BUFFERS,1,
	//GLX_SAMPLES,4,
			None }; // array must be terminated by 0

	gpDisplay = XOpenDisplay(NULL);

	if (gpDisplay == NULL) {
		fprintf(gpFile, "ERROR: Unable to open display terminal");
		uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	//gpXVisualInfo = glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);

	pGLXFBConfigs = glXChooseFBConfig(gpDisplay, XDefaultScreen(gpDisplay),
			frameBufferAttributes, &iNumFBConfigs);

	printf("%d Matching FB Configs Found.\n", iNumFBConfigs);

	// pick that FB config/visual with the most samples per pixel
	int bestFramebufferconfig = -1, worstFramebufferConfig = -1,
			bestNumberOfSamples = -1, worstNumberOfSamples = 999;
	for (i = 0; i < iNumFBConfigs; i++) {
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay,
				pGLXFBConfigs[i]);
		if (pTempXVisualInfo) {
			int sampleBuffer, samples;
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfigs[i],
			GLX_SAMPLE_BUFFERS, &sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfigs[i], GLX_SAMPLES,
					&samples);
			printf(
					"Matching Framebuffer Config=%d : Visual ID=0x%lu : SAMPLE_BUFFERS=%d : SAMPLES=%d\n",
					i, pTempXVisualInfo->visualid, sampleBuffer, samples);
			if (bestFramebufferconfig < 0
					|| sampleBuffer && samples > bestNumberOfSamples) {
				bestFramebufferconfig = i;
				bestNumberOfSamples = samples;
			}
			if (worstFramebufferConfig < 0 || !sampleBuffer
					|| samples < worstNumberOfSamples) {
				worstFramebufferConfig = i;
				worstNumberOfSamples = samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	bestGLXFBConfig = pGLXFBConfigs[bestFramebufferconfig];
	// set global GLXFBConfig
	gFBConfig = bestGLXFBConfig;

	// be sure to free FBConfig list allocated by glXChooseFBConfig()
	XFree(pGLXFBConfigs);

	gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGLXFBConfig);
	printf("Chosen Visual ID=0x%lu\n", gpXVisualInfo->visualid);

	winAttributes.border_pixel = 0;
	winAttributes.background_pixmap = 0;
	Window rootWindow = RootWindow(gpDisplay, defaultScreen);
	winAttributes.colormap = XCreateColormap(gpDisplay, rootWindow,
			gpXVisualInfo->visual, AllocNone);

	gColormap = winAttributes.colormap;
	winAttributes.background_pixel = BlackPixel(gpDisplay, defaultScreen);
	winAttributes.event_mask = ExposureMask |
	VisibilityChangeMask |
	ButtonPressMask |
	KeyPressMask |
	PointerMotionMask |
	StructureNotifyMask;
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow = XCreateWindow(gpDisplay, rootWindow, 0, 0, giWindowWidht,
			giWindowHeight, 0, gpXVisualInfo->depth,
			InputOutput, gpXVisualInfo->visual, styleMask, &winAttributes);
	if (!gWindow) {
		fprintf(gpFile, "ERROR: Fail to create window.\n");
		uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay, gWindow, "Rohit: XWindow OpenGL 3D Rotation");
	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
	XMapWindow(gpDisplay, gWindow);
	fprintf(gpFile, "Exiting from the CreateWindow()\n");
}

void initialize() {
	fprintf(gpFile, "Entered in the Initialize()\n");
	void resize(int, int);

	void LoadGLTexture(const char *, GLuint *);

	glXCreateContextAttribsARB =
			(glXCreateContextAttribsARBProc) glXGetProcAddressARB(
					(GLubyte *) "glXCreateContextAttribsARB");

	GLint attribs[] = {
	GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
	GLX_CONTEXT_MINOR_VERSION_ARB, 5,
	GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB, 0 };

	gGLXContext = glXCreateContextAttribsARB(gpDisplay, gFBConfig, 0, True,
			attribs);

	if (!gGLXContext) {
		gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	} else {
		fprintf(gpFile, "OpenGL 4.5 context is created successfully.\n\n");
	}
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	LoadGLTexture("Smiley.bmp", &Texture_Smiley);

	resize(giWindowWidht, giWindowHeight);
	fprintf(gpFile, "Exiting from the Initialize()\n");
}

void LoadGLTexture(const char *fileName, GLuint* texture) {
	int width, height;
	unsigned char* image;
	int iStatus = 1;

	glGenTextures(1, texture);
	image = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGB);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT,
	GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
}

void toggleFullScreen(void) {
	fprintf(gpFile, "Entered in the ToggleFullScreen()\n");
	Atom wm_state;
	Atom fullScreen;
	XEvent xEvent = { 0 };

	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);

	memset(&xEvent, 0, sizeof(xEvent));

	xEvent.type = ClientMessage;
	xEvent.xclient.window = gWindow;
	xEvent.xclient.message_type = wm_state;
	xEvent.xclient.format = 32;

	if (gbFullScreen == false) {
		xEvent.xclient.data.l[0] = 1;
		gbFullScreen = true;
	} else {
		xEvent.xclient.data.l[0] = 0;
		gbFullScreen = false;
	}

	fullScreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);

	xEvent.xclient.data.l[1] = fullScreen;

	XSendEvent(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), False,
	StructureNotifyMask, &xEvent);

	fprintf(gpFile, "Exiting from the ToggleFullScreen()\n");
}

void display(void) {
	void drawColoredCube(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, (GLfloat) -6.0);
	glRotatef(rotateSquareAngle, 0.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
	drawColoredCube();
	//glFlush();
	glXSwapBuffers(gpDisplay, gWindow);
}

void drawColoredCube() {

	glBegin(GL_QUADS);
	// front
	if (gNumKeyPressed == 1) {
		glTexCoord2f(0.5f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);  //rt

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);  //lt

		glTexCoord2f(0.0f, 0.5f);
		glVertex3f(-1.0f, -1.0f, 1.0f);  //lb

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, -1.0f, 1.0f);  //rt
	} else if (gNumKeyPressed == 2) {
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);  //rt

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);  //lt

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);  //lb

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);  //rt
	} else if (gNumKeyPressed == 3) {
		glTexCoord2f(2.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);  //rt

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);  //lt

		glTexCoord2f(0.0f, 2.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);  //lb

		glTexCoord2f(2.0f, 2.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);  //rt
	} else if (gNumKeyPressed == 4) {
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, 1.0f, 1.0f);  //rt

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 1.0f);  //lt

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, -1.0f, 1.0f);  //lb

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, -1.0f, 1.0f);  //rt
	}

	// right
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);  //rt

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);  //lt

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);  //lb

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);  //rb

	// back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);  //rt

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);  //lt

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);  //lb

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);  //rb

	// left
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); // rt

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f); //lt

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //lb

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); //rb

	// top
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f); // RT

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f); //LT

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); //LB

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f); //RB

	// bottom
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); // RT

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //LT

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); //LB

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //RB

	glEnd();

}

void rotate() {
	if (rotateTriangleAngle == 360.0f)
		rotateTriangleAngle = 0.0f;
	rotateTriangleAngle = rotateTriangleAngle + 0.1f;

	if (rotateSquareAngle == 0.0f)
		rotateSquareAngle = 360.0f;
	rotateSquareAngle = rotateSquareAngle - 0.1f;
}

void resize(int width, int height) {
	if (height == 0) {
		height = 1;
	}
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width <= height)
		gluPerspective(45.0f, (GLfloat) height / (GLfloat) width, 0.1f, 100.0f);
	else
		gluPerspective(45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void uninitialize(void) {


	GLXContext currentGLXContext;
	currentGLXContext = glXGetCurrentContext();

	if (currentGLXContext != NULL && currentGLXContext == gGLXContext) {
		glXMakeCurrent(gpDisplay, 0, 0);
	}

	if (gGLXContext) {
		glXDestroyContext(gpDisplay, gGLXContext);
	}

	if (gWindow) {
		XDestroyWindow(gpDisplay, gWindow);
	}
	if (gColormap) {
		XFreeColormap(gpDisplay, gColormap);
	}
	if (gpXVisualInfo) {
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}
	if (gpDisplay) {
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}

