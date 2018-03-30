//headers
#include <iostream>
#include<GL/glew.h>
#include <stdio.h> //for printf()
#include <stdlib.h> //for exit()
#include <memory.h> //for memset()

//headers for XServer
#include <X11/Xlib.h> //analogous to windows.h
#include <X11/Xutil.h> //for visuals
#include <X11/XKBlib.h> //XkbKeycodeToKeysym()
#include <X11/keysym.h> //for 'Keysym'

#include <GL/gl.h>
#include <GL/glx.h> //for 'glx' functions

#include "vmath.h"
#include "Sphere.h"

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

//global variable declarations
FILE *gpFile = NULL;

Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB=NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext; //parallel to HGLRC

bool gbFullscreen = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;


GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint gModelMatrixUniform, gViewMatrixUniform, gProjectionMatrixUniform;

GLuint gLdUniform;
GLuint gLaUniform;
GLuint gLsUniform;
GLuint gLpUniform;
 
GLuint gKdUniform;
GLuint gKaUniform;
GLuint gKsUniform;
GLuint gKShininessUniform;

int gNumElements;
int gNumVertices;

GLuint gLKeyPressedUniform;


mat4 gPerspectiveProjectionMatrix;

bool gbLight;

GLfloat light_ambient[]={0.5f,0.5f,0.5f,1.0f};
GLfloat light_defused[]={1.0f,1.0f,1.0f,1.0f};
GLfloat light_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat light_position[]={0.0f,0.0f,1.0f,0.0f};

GLfloat s1_material_ambient[]={0.0215f,0.1745f,0.0215f,1.0f};
GLfloat s1_material_diffuse[]={0.07568f,0.61424f,0.07568f,1.0f};
GLfloat s1_material_specular[]={0.633f,0.727811f,0.633f,1.0f};
GLfloat s1_material_shininess=0.6f * 128;

GLfloat s2_material_ambient[]={0.135f,0.2225f,0.1575f,1.0f};
GLfloat s2_material_diffuse[]={0.54f,0.89f,0.63f,1.0f};
GLfloat s2_material_specular[]={0.316228f,0.316228f,0.316228f,1.0f};
GLfloat s2_material_shininess=0.1f * 128;

GLfloat s3_material_ambient[]={0.05375f,0.05f,0.06625f,1.0f};
GLfloat s3_material_diffuse[]={0.18275f,0.17f,0.22525f,1.0f};
GLfloat s3_material_specular[]={0.332741f,0.328634f,0.346435f,1.0f};
GLfloat s3_material_shininess=0.3f * 128;

GLfloat s4_material_ambient[]={0.25f,0.20725f,0.20725f,1.0f};
GLfloat s4_material_diffuse[]={1.0f,0.829f,0.829f,1.0f};
GLfloat s4_material_specular[]={0.296648f,0.296648f,0.296648f,1.0f};
GLfloat s4_material_shininess=0.088f * 128;

GLfloat s5_material_ambient[]={0.1745f,0.01175f,0.01175f,1.0f};
GLfloat s5_material_diffuse[]={0.61424f,0.04136f,0.04136f,1.0f};
GLfloat s5_material_specular[]={0.727811f,0.626959f,0.626959f,1.0f};
GLfloat s5_material_shininess=0.6f * 128;

GLfloat s6_material_ambient[]={0.1f,0.18725f,0.1745f,1.0f};
GLfloat s6_material_diffuse[]={0.396f,0.74151f,0.69102f,1.0f};
GLfloat s6_material_specular[]={0.297254f,0.30829f,0.306678f,1.0f};
GLfloat s6_material_shininess=0.1f * 128;

GLfloat s7_material_ambient[]={0.329412f,0.223529f,0.027451f,1.0f};
GLfloat s7_material_diffuse[]={0.780392f,0.568627f,0.113725f,1.0f};
GLfloat s7_material_specular[]={0.992157f,0.941176f,0.807843f,1.0f};
GLfloat s7_material_shininess=0.21794872f * 128;

GLfloat s8_material_ambient[]={0.2125f,0.1275f,0.054f,1.0f};
GLfloat s8_material_diffuse[]={0.714f,0.4284f,0.18144f,1.0f};
GLfloat s8_material_specular[]={0.393548f,0.271906f,0.166721f,1.0f};
GLfloat s8_material_shininess=0.2f * 128;

GLfloat s9_material_ambient[]={0.25f,0.25f,0.25f,1.0f};
GLfloat s9_material_diffuse[]={0.4f,0.4f,0.4f,1.0f};
GLfloat s9_material_specular[]={0.774597f,0.774597f,0.774597f,1.0f};
GLfloat s9_material_shininess=0.6f * 128;

GLfloat s10_material_ambient[]={0.19125f,0.0735f,0.0225f,1.0f};
GLfloat s10_material_diffuse[]={0.7038f,0.27048f,0.0828f,1.0f};
GLfloat s10_material_specular[]={0.256777f,0.137622f,0.086014f,1.0f};
GLfloat s10_material_shininess=0.1f * 128;

GLfloat s11_material_ambient[]={0.24725f,0.1995f,0.0745f,1.0f};
GLfloat s11_material_diffuse[]={0.75164f,0.60648f,0.22648f,1.0f};
GLfloat s11_material_specular[]={0.628281f,0.555802f,0.366065f,1.0f};
GLfloat s11_material_shininess=0.4f * 128;

GLfloat s12_material_ambient[]={0.19225f,0.19225f,0.19225f,1.0f};
GLfloat s12_material_diffuse[]={0.50754f,0.50754f,0.50754f,1.0f};
GLfloat s12_material_specular[]={0.508273f,0.508273f,0.508273f,1.0f};
GLfloat s12_material_shininess=0.4f * 128;

GLfloat s13_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s13_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
GLfloat s13_material_specular[]={0.50f,0.50f,0.50f,1.0f};
GLfloat s13_material_shininess=0.25f * 128;

GLfloat s14_material_ambient[]={0.0f,0.1f,0.06f,1.0f};
GLfloat s14_material_diffuse[]={0.0f,0.50980392f,0.50980392f,1.0f};
GLfloat s14_material_specular[]={0.50196078f,0.50196078f,0.50196078f,1.0f};
GLfloat s14_material_shininess=0.25f * 128;

GLfloat s15_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s15_material_diffuse[]={0.1f,0.35f,0.1f,1.0f};
GLfloat s15_material_specular[]={0.45f,0.55f,0.45f,1.0f};
GLfloat s15_material_shininess=0.25f * 128;

GLfloat s16_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s16_material_diffuse[]={0.5f,0.0f,0.0f,1.0f};
GLfloat s16_material_specular[]={0.7f,0.6f,0.6f,1.0f};
GLfloat s16_material_shininess=0.25f * 128;

GLfloat s17_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s17_material_diffuse[]={0.55f,0.55f,0.55f,1.0f};
GLfloat s17_material_specular[]={0.70f,0.70f,0.70f,1.0f};
GLfloat s17_material_shininess=0.25f * 128;

GLfloat s18_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat s18_material_diffuse[]={0.5f,0.5f,0.0f,1.0f};
GLfloat s18_material_specular[]={0.60f,0.60f,0.50f,1.0f};
GLfloat s18_material_shininess=0.25f * 128;

GLfloat s19_material_ambient[]={0.02f,0.02f,0.02f,1.0f};
GLfloat s19_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
GLfloat s19_material_specular[]={0.4f,0.4f,0.4f,1.0f};
GLfloat s19_material_shininess=0.078125f * 128;

GLfloat s20_material_ambient[]={0.0f,0.05f,0.05f,1.0f};
GLfloat s20_material_diffuse[]={0.4f,0.5f,0.5f,1.0f};
GLfloat s20_material_specular[]={0.04f,0.7f,0.7f,1.0f};
GLfloat s20_material_shininess=0.078125f * 128;

GLfloat s21_material_ambient[]={0.0f,0.05f,0.0f,1.0f};
GLfloat s21_material_diffuse[]={0.4f,0.5f,0.4f,1.0f};
GLfloat s21_material_specular[]={0.04f,0.7f,0.04f,1.0f};
GLfloat s21_material_shininess=0.078125f * 128;

GLfloat s22_material_ambient[]={0.05f,0.0f,0.0f,1.0f};
GLfloat s22_material_diffuse[]={0.5f,0.4f,0.4f,1.0f};
GLfloat s22_material_specular[]={0.7f,0.04f,0.04f,1.0f};
GLfloat s22_material_shininess=0.078125f * 128;

GLfloat s23_material_ambient[]={0.05f,0.05f,0.05f,1.0f};
GLfloat s23_material_diffuse[]={0.5f,0.5f,0.5f,1.0f};
GLfloat s23_material_specular[]={0.7f,0.7f,0.7f,1.0f};
GLfloat s23_material_shininess=0.078125f * 128;

GLfloat s24_material_ambient[]={0.05f,0.05f,0.0f,1.0f};
GLfloat s24_material_diffuse[]={0.5f,0.5f,0.4f,1.0f};
GLfloat s24_material_specular[]={0.7f,0.7f,0.04f,1.0f};
GLfloat s24_material_shininess=0.078125f * 128;

float zAxis = -15.0f;
float gRotateRedAngle = 0.0f;
bool gbXPressed=false;
bool gbYPressed=false;
bool gbZPressed=false;

float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    unsigned short sphere_elements[2280];
    
    

const GLchar *vertexShaderSourceCodeForPerFragment;

const GLchar *fragmentShaderSourceCodeForPerFragment;

    
//entry-point function
int main(int argc, char *argv[])
{
	//function prototype
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void initialize(void);
	void resize(int,int);
	void display(void);
	void uninitialize(void);
	void rotate(void);
	
	//code
	// create log file
	gpFile=fopen("Log.txt", "w");
	if (gpFile==NULL)
	{
		printf("Log File Can Not Be Created. EXitting Now ...\n");
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}
	
	// create the window
	CreateWindow();
	
	//initialize()
	initialize();
	
	//Message Loop

	//variable declarations
	XEvent event; //parallel to 'MSG' structure
	KeySym keySym;
	int winWidth;
	int winHeight;
	bool bDone=false;
	
	//variable declarations
	
	static bool bIsLKeyPressed = false;
	
	while(bDone==false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay,&event); //parallel to GetMessage()
			switch(event.type) //parallel to 'iMsg'
			{
				case MapNotify: //parallel to WM_CREATE
					break;
				case KeyPress: //parallel to WM_KEYDOWN
					keySym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keySym)
					{
						case XK_Q:
						case XK_q:
							bDone=true;
							break;
						case XK_Escape:
							if(gbFullscreen==false)
							{
								ToggleFullscreen();
								gbFullscreen=true;
							}
							else
							{
								ToggleFullscreen();
								gbFullscreen=false;
							}
							break;
						
						case XK_L: // for 'L' or 'l'
						case XK_l:
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
							
						case XK_W:
						case XK_w:
							gbXPressed=false;
							gbYPressed=false;
							gbZPressed=false;
						break;
						case XK_X:
						case XK_x:
							gbXPressed=true;
							gbYPressed=false;
							gbZPressed=false;
						break;
						case XK_Y:
						case XK_y:
							gbXPressed=false;
							gbYPressed=true;
							gbZPressed=false;
						break;
						case XK_Z:
						case XK_z:
							gbXPressed=false;
							gbYPressed=false;
							gbZPressed=true;
						break;
						default:
							break;
					}
					break;
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1: //Left Button
							break;
						case 2: //Middle Button
							break;
						case 3: //Right Button
							break;
						default: 
							break;
					}
					break;
				case MotionNotify: //parallel to WM_MOUSEMOVE
					break;
				case ConfigureNotify: //parallel to WM_SIZE
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
				case Expose: //parallel to WM_PAINT
					break;
				case DestroyNotify:
					break;
				case 33: //close button, system menu -> close
					bDone=true;
					break;
				default:
					break;
			}
		}
		rotate();
		display();
	}
	
	uninitialize();
	return(0);
}

void rotate() {

        gRotateRedAngle = gRotateRedAngle + 0.1f;
        if (gRotateRedAngle >= 360) {
            gRotateRedAngle = 0.0f;
        }

        float angleRedRadian = (float) (3.14f * gRotateRedAngle / 180.0f);

        if (gbXPressed==true) {
            light_position[0] = 0.0f;
            light_position[1] = 100.0f * ((float) cos(angleRedRadian) / 2.0f - (float) sin(angleRedRadian) / 2.0f);
            light_position[2] = 100.0f * ((float) cos(angleRedRadian) / 2.0f + (float) sin(angleRedRadian) / 2.0f);
            light_position[3] = 1.0f;
        } else if (gbYPressed==true) {
            light_position[0] =100.0f * ((float) cos(angleRedRadian)/2.0f + (float) sin(angleRedRadian)/2.0f);
            light_position[1] =0.0f;
            light_position[2] =100.0f * ((float) cos(angleRedRadian)/2.0f - (float) sin(angleRedRadian)/2.0f);
            light_position[3] =1.0f;
        } else if (gbZPressed==true) {
            light_position[0] =100.0f * ((float) cos(angleRedRadian)/2.0f + (float) sin(angleRedRadian)/2.0f);
            light_position[1] =100.0f * ((float) cos(angleRedRadian)/2.0f - (float) sin(angleRedRadian)/2.0f);
            light_position[2] =0.0f;
            light_position[3] =1.0f;
        }



    }


void CreateWindow(void)
{
	//function prototype
	void uninitialize(void);
	
	//variable declarations
	XSetWindowAttributes winAttribs;
	GLXFBConfig *pGLXFBConfigs=NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo=NULL;
	int iNumFBConfigs=0;
	int styleMask;
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
		None}; // array must be terminated by 0
	
	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		fprintf(gpFile,"ERROR : Unable To Obtain X Display.\n");
		uninitialize();
		exit(1);
	}
	
	// get a new framebuffer config that meets our attrib requirements
	pGLXFBConfigs=glXChooseFBConfig(gpDisplay,DefaultScreen(gpDisplay),frameBufferAttributes,&iNumFBConfigs);
	if(pGLXFBConfigs==NULL)
	{
		fprintf(gpFile,"Failed To Get Valid Framebuffer Config. Exitting Now ...\n");
		uninitialize();
		exit(1);
	}
	fprintf(gpFile,"%d Matching FB Configs Found.\n",iNumFBConfigs);
	
	// pick that FB config/visual with the most samples per pixel
	int bestFramebufferconfig=-1,worstFramebufferConfig=-1,bestNumberOfSamples=-1,worstNumberOfSamples=999;
	for(i=0;i<iNumFBConfigs;i++)
	{
		pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfigs[i]);
		if(pTempXVisualInfo)
		{
			int sampleBuffer,samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLE_BUFFERS,&sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLES,&samples);
			fprintf(gpFile,"Matching Framebuffer Config=%d : Visual ID=0x%lu : SAMPLE_BUFFERS=%d : SAMPLES=%d\n",i,pTempXVisualInfo->visualid,sampleBuffer,samples);
			if(bestFramebufferconfig < 0 || sampleBuffer && samples > bestNumberOfSamples)
			{
				bestFramebufferconfig=i;
				bestNumberOfSamples=samples;
			}
			if( worstFramebufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
			{
				worstFramebufferConfig=i;
			    worstNumberOfSamples=samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	bestGLXFBConfig = pGLXFBConfigs[bestFramebufferconfig];
	// set global GLXFBConfig
	gGLXFBConfig=bestGLXFBConfig;
	
	// be sure to free FBConfig list allocated by glXChooseFBConfig()
	XFree(pGLXFBConfigs);
	
	gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,bestGLXFBConfig);
	fprintf(gpFile,"Chosen Visual ID=0x%lu\n",gpXVisualInfo->visualid );
	
	//setting window's attributes
	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,
										RootWindow(gpDisplay,gpXVisualInfo->screen), //you can give defaultScreen as well
										gpXVisualInfo->visual,
										AllocNone); //for 'movable' memory allocation
										
	winAttribs.event_mask=StructureNotifyMask | KeyPressMask | ButtonPressMask |
						  ExposureMask | VisibilityChangeMask | PointerMotionMask;
	
	styleMask=CWBorderPixel | CWEventMask | CWColormap;
	gColormap=winAttribs.colormap;										           
	
	gWindow=XCreateWindow(gpDisplay,
						  RootWindow(gpDisplay,gpXVisualInfo->screen),
						  0,
						  0,
						  WIN_WIDTH,
						  WIN_HEIGHT,
						  0, //border width
						  gpXVisualInfo->depth, //depth of visual (depth for Colormap)          
						  InputOutput, //class(type) of your window
						  gpXVisualInfo->visual,
						  styleMask,
						  &winAttribs);
	if(!gWindow)
	{
		fprintf(gpFile,"Failure In Window Creation.\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay,gWindow,"OpenGL Window");
	
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_WINDOW_DELETE",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
}

void ToggleFullscreen(void)
{
	//code
	Atom wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False); //normal window state
	
	XEvent event;
	memset(&event,0,sizeof(XEvent));
	
	event.type=ClientMessage;
	event.xclient.window=gWindow;
	event.xclient.message_type=wm_state;
	event.xclient.format=32; //32-bit
	event.xclient.data.l[0]=gbFullscreen ? 0 : 1;

	Atom fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);	
	event.xclient.data.l[1]=fullscreen;
	
	//parallel to SendMessage()
	XSendEvent(gpDisplay,
			   RootWindow(gpDisplay,gpXVisualInfo->screen),
			   False, //do not send this message to Sibling windows
			   StructureNotifyMask, //resizing mask (event_mask)
			   &event);	
}

void initialize(void)
{
	// function declarations
	void uninitialize(void);
	void resize(int,int);
	void initVertexShader(void);
	void initFragmentShader(void);
	void initShaderProgramObject(void);
	void drawSphere(void);
	
	//code
	// create a new GL context 4.5 for rendering
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
	
	GLint attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB,4,
		GLX_CONTEXT_MINOR_VERSION_ARB,5,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0 }; // array must be terminated by 0
		
	gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);

	if(!gGLXContext) // fallback to safe old style 2.x context
	{
		// When a context version below 3.0 is requested, implementations will return 
		// the newest context version compatible with OpenGL versions less than version 3.0.
		GLint attribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB,1,
			GLX_CONTEXT_MINOR_VERSION_ARB,0,
			0 }; // array must be terminated by 0
		fprintf(gpFile,"Failed To Create GLX 4.5 context. Hence Using Old-Style GLX Context\n");
		gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	}
	else // successfully created 4.1 context
	{
		fprintf(gpFile,"OpenGL Context 4.5 Is Created.\n");
	}
	
	// verifying that context is a direct context
	if(!glXIsDirect(gpDisplay,gGLXContext))
	{
		fprintf(gpFile,"Indirect GLX Rendering Context Obtained\n");
	}
	else
	{
		fprintf(gpFile,"Direct GLX Rendering Context Obtained\n" );
	}
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	//code to find supported OpenGL Extensions
	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK)
	{
		fprintf(gpFile, "Can not initialize glew.\n");
		uninitialize();
	}
	fprintf(gpFile, "GLEW initialization done successfully.\n");
	
	
	
	
	//provide source code to shader
	vertexShaderSourceCodeForPerFragment =
				"#version 440							"\
				"\n										"\
				"in vec4 vPosition;						"\
				"in vec3 vNormal;"\
				"uniform mat4 u_model_matrix;"\
				"uniform mat4 u_view_matrix;"\
				"uniform mat4 u_projection_matrix;"\
				"uniform int u_LKeyPressed;"\
				"uniform vec3 u_Lp;"\
				"out vec3 transformed_normals;"\
				"out vec3 light_direction;"\
				"out vec3 viewer_vector;"\
				"void main(void)						"\
				"{										"\
				"if(u_LKeyPressed == 1)"\
				"{"\
				"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"\
				"transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;"\
				"light_direction = vec3(u_Lp) - eyeCoordinates.xyz;"\
				"viewer_vector = -eyeCoordinates.xyz;"\
				"}"\
				"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;	"\
				"}										";
				
	fragmentShaderSourceCodeForPerFragment =
				"#version 440" \
				"\n" \
				"in vec3 transformed_normals;"\
				"in vec3 light_direction;"\
				"in vec3 viewer_vector;"\
				"out vec4 FragColor;" \
				"uniform vec3 u_La;"\
				"uniform vec3 u_Ld;"\
				"uniform vec3 u_Ls;"\
				"uniform vec3 u_Ka;"\
				"uniform vec3 u_Kd;"\
				"uniform vec3 u_Ks;"\
				"uniform float u_KShininess;"\
				"uniform int u_LKeyPressed;"
				"void main(void)" \
				"{" \
				"vec3 phong_ads_color;"\
				"if(u_LKeyPressed==1)"\
				"{"\
				"vec3 normalized_transformed_normals=normalize(transformed_normals);"\
				"vec3 normalized_light_direction=normalize(light_direction);"\
				"vec3 normalized_viewer_vector=normalize(viewer_vector);"\
				"vec3 ambient=u_La * u_Ka;"\
				"float tn_dot_id = max(dot(normalized_transformed_normals,normalized_light_direction),0.0);"\
				"vec3 diffuse = u_Ld * u_Kd * tn_dot_id;"\
				"vec3 reflection_vector=reflect(-normalized_light_direction, normalized_transformed_normals);"\
				"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector),0.0), u_KShininess);"\
				"phong_ads_color=ambient+diffuse+specular;"\
				"}"\
				"else"\
				"{"\
				"phong_ads_color=vec3(1.0,1.0,1.0);"\
				"}"\
				"FragColor=vec4(phong_ads_color, 1.0);" \
				"}";
				
				
	initVertexShader();
	initFragmentShader();
	initShaderProgramObject();
				
	//get MVP uniform location
	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	
	gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	
	gLaUniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	gLdUniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	gLsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
	gLpUniform = glGetUniformLocation(gShaderProgramObject, "u_Lp");
	
	gKaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	gKdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	gKsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	gKShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_KShininess");
	
	drawSphere();
	
	
	
	
	
	
	//code
	glShadeModel(GL_SMOOTH);
	// set-up depth buffer
	glClearDepth(1.0f);
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// depth test to do
	glDepthFunc(GL_LEQUAL);
	// set really nice percpective calculations ?
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	// We will always cull back faces for better performance
	//glEnable(GL_CULL_FACE);

	// set background clearing color
	glClearColor(0.0f,0.0f,0.0f,0.0f); // blue
	
	//set orthographicMatrix to identify matrix
	gPerspectiveProjectionMatrix = mat4::identity();
	
	// resize
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void initVertexShader(void)
{
	void uninitialize(void);
	//VERTEX SHADER
	//create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	
	
		glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCodeForPerFragment,NULL);
	
	
	//compile shader
	//fprintf(gpFile, "compiling vertex shader.\n");
	glCompileShader(gVertexShaderObject);
	//fprintf(gpFile, "vertex shader compiled.\n");
	
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
				fprintf(gpFile, "vertex shader compilation log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
}

void initFragmentShader(void)
{
	void uninitialize(void);
	//FRAGMENT SHADER
	//create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	
	
		glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCodeForPerFragment,NULL);
	
	
	//compile shader
	//fprintf(gpFile, "compiling fragment shader.\n");
	glCompileShader(gFragmentShaderObject);
	//fprintf(gpFile, "fragment shader compiled.\n");
	
	GLint iInfoLogLength=0;
	GLint iShaderCompileStatus=0;
	char *szInfoLog=NULL;
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
				fprintf(gpFile,"Fragment Shader Compilation Log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
}

void initShaderProgramObject(void)
{
	void uninitialize(void);
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
	//fprintf(gpFile, "shader object linked.\n");
	
	GLint iInfoLogLength=0;
	char *szInfoLog=NULL;
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
				fprintf(gpFile, "Shader Program Link Log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
}


void drawSphere(){
	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();
	// vao
    glGenVertexArrays(1, &gVao_sphere);
    glBindVertexArray(gVao_sphere);

    // position vbo
    glGenBuffers(1, &gVbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // normal vbo
    glGenBuffers(1, &gVbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

    glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // element vbo
    glGenBuffers(1, &gVbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // unbind vao
    glBindVertexArray(0);
}


void resize(int width,int height)
{
    //code
	if(height == 0){
		height = 1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	
	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f,100.0f);
}

void display(void)
{
	void initVertexShader(void);
	void initFragmentShader(void);
	void initShaderProgramObject(void);
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	
	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);
	
	if(gbLight==true)
	{
		glUniform1i(gLKeyPressedUniform, 1);
		glUniform3fv(gLaUniform, 1, light_ambient);
		glUniform3fv(gLdUniform, 1, light_defused);
		glUniform3fv(gLsUniform, 1, light_specular);
		glUniform3fv(gLpUniform, 1, light_position);
	}else
	{
		glUniform1i(gLKeyPressedUniform,0);
	}
	
	//OpenGL drawing
	//set modelview & modelviewprojection matrices to identity
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	
	//1st row
	glUniform3fv(gKaUniform, 1, s1_material_ambient);
	glUniform3fv(gKdUniform, 1, s1_material_diffuse);
	glUniform3fv(gKsUniform, 1, s1_material_specular);
	glUniform1f(gKShininessUniform, s1_material_shininess);
	//translate
	modelMatrix = translate(-3.75f, 3.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s2_material_ambient);
	glUniform3fv(gKdUniform, 1, s2_material_diffuse);
	glUniform3fv(gKsUniform, 1, s2_material_specular);
	glUniform1f(gKShininessUniform, s2_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(-1.25f, 3.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s24_material_ambient);
	glUniform3fv(gKdUniform, 1, s24_material_diffuse);
	glUniform3fv(gKsUniform, 1, s24_material_specular);
	glUniform1f(gKShininessUniform, s2_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(1.25f, 3.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	
	glUniform3fv(gKaUniform, 1, s3_material_ambient);
	glUniform3fv(gKdUniform, 1, s3_material_diffuse);
	glUniform3fv(gKsUniform, 1, s3_material_specular);
	glUniform1f(gKShininessUniform, s3_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(3.75f, 3.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	
	
	
	
	//2nd row
	//translate
	glUniform3fv(gKaUniform, 1, s4_material_ambient);
	glUniform3fv(gKdUniform, 1, s4_material_diffuse);
	glUniform3fv(gKsUniform, 1, s4_material_specular);
	glUniform1f(gKShininessUniform, s4_material_shininess);
	modelMatrix = translate(-3.75f, 2.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	
	glUniform3fv(gKaUniform, 1, s5_material_ambient);
	glUniform3fv(gKdUniform, 1, s5_material_diffuse);
	glUniform3fv(gKsUniform, 1, s5_material_specular);
	glUniform1f(gKShininessUniform, s5_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(-1.25f, 2.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s6_material_ambient);
	glUniform3fv(gKdUniform, 1, s6_material_diffuse);
	glUniform3fv(gKsUniform, 1, s6_material_specular);
	glUniform1f(gKShininessUniform, s6_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(1.25f, 2.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	//translate
	glUniform3fv(gKaUniform, 1, s7_material_ambient);
	glUniform3fv(gKdUniform, 1, s7_material_diffuse);
	glUniform3fv(gKsUniform, 1, s7_material_specular);
	glUniform1f(gKShininessUniform, s7_material_shininess);
	modelMatrix = mat4::identity();
	modelMatrix = translate(3.75f, 2.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	
	
	
	
	//3rd row
	glUniform3fv(gKaUniform, 1, s8_material_ambient);
	glUniform3fv(gKdUniform, 1, s8_material_diffuse);
	glUniform3fv(gKsUniform, 1, s8_material_specular);
	glUniform1f(gKShininessUniform, s8_material_shininess);
	//translate
	modelMatrix = translate(-3.75f, 1.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	//translate
	glUniform3fv(gKaUniform, 1, s9_material_ambient);
	glUniform3fv(gKdUniform, 1, s9_material_diffuse);
	glUniform3fv(gKsUniform, 1, s9_material_specular);
	glUniform1f(gKShininessUniform, s9_material_shininess);
	modelMatrix = mat4::identity();
	modelMatrix = translate(-1.25f, 1.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s10_material_ambient);
	glUniform3fv(gKdUniform, 1, s10_material_diffuse);
	glUniform3fv(gKsUniform, 1, s10_material_specular);
	glUniform1f(gKShininessUniform, s10_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(1.25f, 1.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s11_material_ambient);
	glUniform3fv(gKdUniform, 1, s11_material_diffuse);
	glUniform3fv(gKsUniform, 1, s11_material_specular);
	glUniform1f(gKShininessUniform, s11_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(3.75f, 1.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	
	
	glUniform3fv(gKaUniform, 1, s12_material_ambient);
	glUniform3fv(gKdUniform, 1, s12_material_diffuse);
	glUniform3fv(gKsUniform, 1, s12_material_specular);
	glUniform1f(gKShininessUniform, s12_material_shininess);
	//4th row
	//translate
	modelMatrix = translate(-3.75f, 0.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s13_material_ambient);
	glUniform3fv(gKdUniform, 1, s13_material_diffuse);
	glUniform3fv(gKsUniform, 1, s13_material_specular);
	glUniform1f(gKShininessUniform, s13_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(-1.25f, 0.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s14_material_ambient);
	glUniform3fv(gKdUniform, 1, s14_material_diffuse);
	glUniform3fv(gKsUniform, 1, s14_material_specular);
	glUniform1f(gKShininessUniform, s14_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(1.25f, 0.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s15_material_ambient);
	glUniform3fv(gKdUniform, 1, s15_material_diffuse);
	glUniform3fv(gKsUniform, 1, s15_material_specular);
	glUniform1f(gKShininessUniform, s15_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(3.75f, 0.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	
	
	glUniform3fv(gKaUniform, 1, s16_material_ambient);
	glUniform3fv(gKdUniform, 1, s16_material_diffuse);
	glUniform3fv(gKsUniform, 1, s16_material_specular);
	glUniform1f(gKShininessUniform, s16_material_shininess);
	//5th row
	//translate
	modelMatrix = translate(-3.75f, -1.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s17_material_ambient);
	glUniform3fv(gKdUniform, 1, s17_material_diffuse);
	glUniform3fv(gKsUniform, 1, s17_material_specular);
	glUniform1f(gKShininessUniform, s17_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(-1.25f, -1.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s18_material_ambient);
	glUniform3fv(gKdUniform, 1, s18_material_diffuse);
	glUniform3fv(gKsUniform, 1, s18_material_specular);
	glUniform1f(gKShininessUniform, s18_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(1.25f, -1.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	glUniform3fv(gKaUniform, 1, s19_material_ambient);
	glUniform3fv(gKdUniform, 1, s19_material_diffuse);
	glUniform3fv(gKsUniform, 1, s19_material_specular);
	glUniform1f(gKShininessUniform, s19_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(3.75f, -1.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	
	
	glUniform3fv(gKaUniform, 1, s20_material_ambient);
	glUniform3fv(gKdUniform, 1, s20_material_diffuse);
	glUniform3fv(gKsUniform, 1, s20_material_specular);
	glUniform1f(gKShininessUniform, s20_material_shininess);
	//6th row
	//translate
	modelMatrix = translate(-3.75f, -2.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s21_material_ambient);
	glUniform3fv(gKdUniform, 1, s21_material_diffuse);
	glUniform3fv(gKsUniform, 1, s21_material_specular);
	glUniform1f(gKShininessUniform, s21_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(-1.25f, -2.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s22_material_ambient);
	glUniform3fv(gKdUniform, 1, s22_material_diffuse);
	glUniform3fv(gKsUniform, 1, s22_material_specular);
	glUniform1f(gKShininessUniform, s22_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(1.25f, -2.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	
	glUniform3fv(gKaUniform, 1, s23_material_ambient);
	glUniform3fv(gKdUniform, 1, s23_material_diffuse);
	glUniform3fv(gKsUniform, 1, s23_material_specular);
	glUniform1f(gKShininessUniform, s23_material_shininess);
	//translate
	modelMatrix = mat4::identity();
	modelMatrix = translate(3.75f, -2.0f, zAxis);
	
	//pass the above modelViewProjectionMatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	
	// *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
	
	//stop using OpenGL program object
	glUseProgram(0);
	
	glXSwapBuffers(gpDisplay,gWindow);
}

void uninitialize(void)
{
	//code
	//destroy vao
	if(gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}
	
	//destroy vbo
	if(gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}
	
	if(gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}
	
	if(gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
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
	
	// Releasing OpenGL related and XWindow related objects 	
	GLXContext currentContext=glXGetCurrentContext();
	if(currentContext!=NULL && currentContext==gGLXContext)
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

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
