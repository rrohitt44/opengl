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

GLuint gL0dUniform;
GLuint gL0aUniform;
GLuint gL0sUniform;
GLuint gL0pUniform;

GLuint gL1dUniform;
GLuint gL1aUniform;
GLuint gL1sUniform;
GLuint gL1pUniform;

GLuint gL2dUniform;
GLuint gL2aUniform;
GLuint gL2sUniform;
GLuint gL2pUniform;
 
GLuint gKdUniform;
GLuint gKaUniform;
GLuint gKsUniform;
GLuint gKShininessUniform;

int gNumElements;
int gNumVertices;

GLuint gLKeyPressedUniform;


mat4 gPerspectiveProjectionMatrix;

bool gbLight;

GLfloat light0_ambient[] = {0.0f,0.0f,0.0f,1.0f};
    GLfloat light0_defused[] = {1.0f,0.0f,0.0f,1.0f};
    GLfloat light0_specular[] = {1.0f,0.0f,0.0f,1.0f};
    GLfloat light0_position[] = {0.0f,0.0f,0.0f,1.0f};

    GLfloat light1_ambient[] = {0.0f,0.0f,0.0f,1.0f};
    GLfloat light1_defused[] = {0.0f,1.0f,0.0f,1.0f};
    GLfloat light1_specular[] = {0.0f,1.0f,0.0f,1.0f};
    GLfloat light1_position[] = {0.0f,0.0f,0.0f,1.0f};

    GLfloat light2_ambient[] = {0.0f,0.0f,0.0f,1.0f};
    GLfloat light2_defused[] = {0.0f,0.0f,1.0f,1.0f};
    GLfloat light2_specular[] = {0.0f,0.0f,1.0f,1.0f};
    GLfloat light2_position[] = {0.0f,0.0f,0.0f,1.0f};

    GLfloat material_ambient[] = {0.0f,0.0f,0.0f,0.0f};
    GLfloat material_defused[] = {1.0f,1.0f,1.0f,1.0f};
    GLfloat material_specular[] = {1.0f,1.0f,1.0f,1.0f};
    GLfloat material_shininess = 50.0f;

GLfloat angleRedLight = 0.0f;
GLfloat angleGreenLight = 0.0f;
GLfloat angleBlueLight = 0.0f;

float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    unsigned short sphere_elements[2280];
    
    
const GLchar *vertexShaderSourceCodeForPerVertex;
const GLchar *vertexShaderSourceCodeForPerFragment;
const GLchar *fragmentShaderSourceCodeForPerVertex;
const GLchar *fragmentShaderSourceCodeForPerFragment;

bool gbFKeyPressed=false;
bool gbVKeyPressed=true;
    
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
	void updateAngle(void);
	
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
						case XK_F:
						case XK_f: //F or f key
								if(gbFKeyPressed==false)
								{	
									gbFKeyPressed = true;
									gbVKeyPressed = false;
								}else
								{
									gbFKeyPressed = false;
									gbVKeyPressed = true;
								}
								fprintf(gpFile,"F Pressed : F key %d and V key %d\n",gbFKeyPressed, gbVKeyPressed);
							break;
						case XK_V:
						case XK_v: //V or v key
								if(gbVKeyPressed==false)
								{	
									gbVKeyPressed = true;
									gbFKeyPressed = false;
								}else
								{
									gbVKeyPressed = false;
									gbFKeyPressed = true;
								}
								fprintf(gpFile,"V Pressed : F key %d and V key %d\n",gbFKeyPressed, gbVKeyPressed);
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
		updateAngle();
		display();
	}
	
	uninitialize();
	return(0);
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
	vertexShaderSourceCodeForPerVertex =
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
				"uniform vec3 u_L2a;"\
				"uniform vec3 u_L2d;"\
				"uniform vec3 u_L2s;"\
				"uniform vec4 u_L2p;"\
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
				
				"vec3 light_direction0= normalize(vec3(u_L0p) - eyeCoordinates.xyz);"\
				"float tn_dot_ld0=max(dot(transformed_normals, light_direction0),0.0);"\
				"vec3 ambient0 = u_L0a * u_Ka;"\
				"vec3 diffuse0 = u_L0d * u_Kd * tn_dot_ld0;"\
				"vec3 reflection_vector0 = reflect(-light_direction0, transformed_normals);"\
				"vec3 viewer_vector0 = normalize(-eyeCoordinates.xyz);"\
				"vec3 specular0 = u_L0s * u_Ks * pow(max(dot(reflection_vector0, viewer_vector0),0.0), u_KShininess);"\
				
				"vec3 light_direction1 = normalize(vec3(u_L1p) - eyeCoordinates.xyz);"\
				"float tn_dot_ld1=max(dot(transformed_normals, light_direction1),0.0);"\
				"vec3 ambient1 = u_L1a * u_Ka;"\
				"vec3 diffuse1 = u_L1d * u_Kd * tn_dot_ld1;"\
				"vec3 reflection_vector1 = reflect(-light_direction1, transformed_normals);"\
				//"vec3 viewer_vector1 = normalize(-eyeCoordinates.xyz);"
				"vec3 specular1 = u_L1s * u_Ks * pow(max(dot(reflection_vector1, viewer_vector0),0.0), u_KShininess);"\
				
				"vec3 light_direction2 = normalize(vec3(u_L2p) - eyeCoordinates.xyz);"\
				"float tn_dot_ld2=max(dot(transformed_normals, light_direction2),0.0);"\
				"vec3 ambient2 = u_L2a * u_Ka;"\
				"vec3 diffuse2 = u_L2d * u_Kd * tn_dot_ld2;"\
				"vec3 reflection_vector2 = reflect(-light_direction2, transformed_normals);"\
				//"vec3 viewer_vector2 = normalize(-eyeCoordinates.xyz);"
				"vec3 specular2 = u_L2s * u_Ks * pow(max(dot(reflection_vector2, viewer_vector0),0.0), u_KShininess);"\
				
				"phong_ads_color=ambient0+diffuse0+specular0 + ambient1+diffuse1+specular1 + ambient2+diffuse2+specular2;"\
				"}"\
				"else"\
				"{"\
				"phong_ads_color=vec3(1.0,1.0,1.0);"\
				"}"\
				"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;	"\
				"}										";
				
	vertexShaderSourceCodeForPerFragment =
				"#version 440							"\
				"\n										"\
				"in vec4 vPosition;						"\
				"in vec3 vNormal;"\
				"uniform mat4 u_model_matrix;"\
				"uniform mat4 u_view_matrix;"\
				"uniform mat4 u_projection_matrix;"\
				"uniform int u_LKeyPressed;"\
				"uniform vec3 u_L0p;"\
				"uniform vec3 u_L1p;"\
				"uniform vec3 u_L2p;"\
				"out vec3 transformed_normals;"\
				"out vec3 light0_direction;"\
				"out vec3 light1_direction;"\
				"out vec3 light2_direction;"\
				"out vec3 viewer_vector;"\
				"void main(void)						"\
				"{										"\
				"if(u_LKeyPressed == 1)"\
				"{"\
				"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"\
				"transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;"\
				"light0_direction = vec3(u_L0p) - eyeCoordinates.xyz;"\
				"light1_direction = vec3(u_L1p) - eyeCoordinates.xyz;"\
				"light2_direction = vec3(u_L2p) - eyeCoordinates.xyz;"\
				"viewer_vector = -eyeCoordinates.xyz;"\
				"}"\
				"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;	"\
				"}										";
				
	
	
	
	
	
	//provide source code to shader
	fragmentShaderSourceCodeForPerVertex =
				"#version 440" \
				"\n" \
				"in vec3 phong_ads_color;"\
				"out vec4 FragColor;" \
				"void main(void)" \
				"{" \
				"FragColor=vec4(phong_ads_color, 1.0);" \
				"}";
				
	fragmentShaderSourceCodeForPerFragment =
				"#version 440" \
				"\n" \
				"in vec3 transformed_normals;"\
				"in vec3 light0_direction;"\
				"in vec3 light1_direction;"\
				"in vec3 light2_direction;"\
				"in vec3 viewer_vector;"\
				"out vec4 FragColor;" \
				"uniform vec3 u_L0a;"\
				"uniform vec3 u_L0d;"\
				"uniform vec3 u_L0s;"\
				"uniform vec3 u_L1a;"\
				"uniform vec3 u_L1d;"\
				"uniform vec3 u_L1s;"\
				"uniform vec3 u_L2a;"\
				"uniform vec3 u_L2d;"\
				"uniform vec3 u_L2s;"\
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
				"vec3 normalized_light0_direction=normalize(light0_direction);"\
				"vec3 normalized_light1_direction=normalize(light1_direction);"\
				"vec3 normalized_light2_direction=normalize(light2_direction);"\
				"vec3 normalized_viewer_vector=normalize(viewer_vector);"\
				
				"vec3 ambient0=u_L0a * u_Ka;"\
				"float tn_dot_id0 = max(dot(normalized_transformed_normals,normalized_light0_direction),0.0);"\
				"vec3 diffuse0 = u_L0d * u_Kd * tn_dot_id0;"\
				"vec3 reflection_vector0=reflect(-normalized_light0_direction, normalized_transformed_normals);"\
				"vec3 specular0 = u_L0s * u_Ks * pow(max(dot(reflection_vector0, normalized_viewer_vector),0.0), u_KShininess);"\
				
				"vec3 ambient1=u_L1a * u_Ka;"\
				"float tn_dot_id1 = max(dot(normalized_transformed_normals,normalized_light1_direction),0.0);"\
				"vec3 diffuse1 = u_L1d * u_Kd * tn_dot_id1;"\
				"vec3 reflection_vector1=reflect(-normalized_light1_direction, normalized_transformed_normals);"\
				"vec3 specular1 = u_L1s * u_Ks * pow(max(dot(reflection_vector1, normalized_viewer_vector),0.0), u_KShininess);"\
				
				"vec3 ambient2=u_L2a * u_Ka;"\
				"float tn_dot_id2 = max(dot(normalized_transformed_normals,normalized_light2_direction),0.0);"\
				"vec3 diffuse2 = u_L2d * u_Kd * tn_dot_id2;"\
				"vec3 reflection_vector2=reflect(-normalized_light2_direction, normalized_transformed_normals);"\
				"vec3 specular2 = u_L2s * u_Ks * pow(max(dot(reflection_vector2, normalized_viewer_vector),0.0), u_KShininess);"\
				
				"phong_ads_color=ambient0+diffuse0+specular0 + ambient1+diffuse1+specular1 + ambient2+diffuse2+specular2;"\
				"}"\
				"else"\
				"{"\
				"phong_ads_color=vec3(1.0,1.0,1.0);"\
				"}"\
				"FragColor=vec4(phong_ads_color, 1.0);" \
				"}";
				
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
	
	if(gbVKeyPressed==true && gbFKeyPressed==false){
		glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCodeForPerVertex,NULL);
	}else if(gbFKeyPressed==true && gbVKeyPressed==false){
		glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCodeForPerFragment,NULL);
	}
	
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
	
	if(gbVKeyPressed==true && gbFKeyPressed==false){
		glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCodeForPerVertex,NULL);
	}else if(gbFKeyPressed==true && gbVKeyPressed==false){
		glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCodeForPerFragment,NULL);
	}
	
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

void updateAngle(void){
		
	angleRedLight = angleRedLight+0.1f;
	if(angleRedLight>=360){
		angleRedLight = 0.0f;
	}
	
	angleGreenLight = angleGreenLight+0.1f;
	if(angleGreenLight>=360){
		angleGreenLight = 0.0f;
	}
	
	angleBlueLight = angleBlueLight+0.1f;
	if(angleBlueLight>=360){
		angleBlueLight = 0.0f;
	}
	
	float angleRedRadian = (float) (3.14f *angleRedLight/180.0f);
        light0_position[0] =0.0f;
        light0_position[1] =100.0f * ((float) cos(angleRedRadian)/2.0f - (float) sin(angleRedRadian)/2.0f);
        light0_position[2] =100.0f * ((float) cos(angleRedRadian)/2.0f + (float) sin(angleRedRadian)/2.0f);
        light0_position[3] =1.0f;

        float angleGreenRadian = (float) (3.14f *angleGreenLight/180.0f);
        light1_position[0] =100.0f * ((float) cos(angleGreenRadian)/2.0f + (float) sin(angleGreenRadian)/2.0f);
        light1_position[1] =0.0f;
        light1_position[2] =100.0f * ((float) cos(angleGreenRadian)/2.0f - (float) sin(angleGreenRadian)/2.0f);
        light1_position[3] =1.0f;

        float angleBlueRadian = (float) (3.14f *angleBlueLight/180.0f);

        light2_position[0] =100.0f * ((float) cos(angleBlueRadian)/2.0f + (float) sin(angleBlueRadian)/2.0f);
        light2_position[1] =100.0f * ((float) cos(angleBlueRadian)/2.0f - (float) sin(angleBlueRadian)/2.0f);
        light2_position[2] =0.0f;
        light2_position[3] =1.0f;
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
	
	initVertexShader();
	initFragmentShader();
	initShaderProgramObject();
	
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
	
	gL2aUniform = glGetUniformLocation(gShaderProgramObject, "u_L2a");
	gL2dUniform = glGetUniformLocation(gShaderProgramObject, "u_L2d");
	gL2sUniform = glGetUniformLocation(gShaderProgramObject, "u_L2s");
	gL2pUniform = glGetUniformLocation(gShaderProgramObject, "u_L2p");
	
	gKaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	gKdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	gKsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	gKShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_KShininess");
	
	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);
	
	if(gbLight==true)
	{
		glUniform1i(gLKeyPressedUniform, 1);
		
		//light0_position[1] = angleRedLight;
		glUniform3fv(gL0aUniform, 1, light0_ambient);
		glUniform3fv(gL0dUniform, 1, light0_defused);
		glUniform3fv(gL0sUniform, 1, light0_specular);
		glUniform3fv(gL0pUniform, 1, light0_position);
		
		//light1_position[0]=angleGreenLight;
		glUniform3fv(gL1aUniform, 1, light1_ambient);
		glUniform3fv(gL1dUniform, 1, light1_defused);
		glUniform3fv(gL1sUniform, 1, light1_specular);
		glUniform3fv(gL1pUniform, 1, light1_position);
		
		//light2_position[0]=angleBlueLight;
		glUniform3fv(gL2aUniform, 1, light2_ambient);
		glUniform3fv(gL2dUniform, 1, light2_defused);
		glUniform3fv(gL2sUniform, 1, light2_specular);
		glUniform3fv(gL2pUniform, 1, light2_position);
		
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
	
	//translate
	modelMatrix = translate(0.0f,0.0f,-3.0f);
	
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
