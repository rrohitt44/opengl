package org.astromedicomp.win.winvm_gles;

import android.content.Context;
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32;
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

// for vbo
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.Matrix; // for Matrix math  

import java.nio.ShortBuffer;

//a view for OpenGLES3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	
	private GestureDetector gestureDetector;
	
	private int gVertexShaderObject;
	private int gFragmentShaderObject;
	private int gShaderProgramObject;
	
	private int gModelMatrixUniform, gViewMatrixUniform, gProjectionMatrixUniform;
	private int gLaUniform, gLdUniform, gLpUniform, gLsUniform, gKaUniform, gKsUniform, gKShininessUniform,gKdUniform;
	private int gLKeyPressedUniform;
	
	private float[] gPerspectiveProjectionMatrix = new float[16];
	
	boolean gbLight;
	boolean gbSingleTap;
	
	private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
	int numVertices;
	int numElements;
	
	float lightAmbient[] = {0.0f,0.0f,0.0f,1.0f};
float lightDiffuse[] = {1.0f,1.0f,1.0f,1.0f};
float lightSpecular[] = {1.0f,1.0f,1.0f,1.0f};
float lightPosition[] = {100.0f,100.0f,100.0f,1.0f};

float materialAmbient[] = {0.0f,0.0f,0.0f,1.0f};
float materialDiffuse[] = {1.0f,1.0f,1.0f,1.0f};
float materialSpecular[] = {1.0f,1.0f,1.0f,1.0f};
float materialShininess = 50.0f;

String vertexShaderSourceCodePerVertext;
String vertexShaderSourceCodePerFragment;
String fragmentShaderSourceCodePerVertex;
String fragmentShaderSourceCodePerFragment;
	public GLESView(Context drawingContext)
	{
		super(drawingContext);
		context=drawingContext;
		
		//accordingly set EGLContext to current supported version of OpenGL-ES
		setEGLContextClientVersion(3);
		
		//set renderer for drawing on the GLSurfaceView
		setRenderer(this);
		
		//Render the view only when there is a change in the drawing data
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		
		gestureDetector = new GestureDetector(context, this, null, false);
		
		gestureDetector.setOnDoubleTapListener(this);
	}
	
	//overriden method of GLSurfaceView.Renderer (init code)
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
        // get OpenGL-ES version
        String glesVersion = gl.glGetString(GL10.GL_VERSION);
        System.out.println("SAM: OpenGL-ES Version = "+glesVersion);
        // get GLSL version
        String glslVersion=gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("SAM: GLSL Version = "+glslVersion);
		initialize(gl);
	}
	
	//overriden method of GLSurfaceView.Renderer (change size code)
	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height)
	{
		resize(width, height);
	}
	
	//Overriden method of GLSurfaceView.Renderer (rendering code)
	@Override
	public void onDrawFrame(GL10 unused)
	{
		display();
	}
	
	//handling onTouchEvent is the most important,
	//because it triggers all gesture and tap events
	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		int eventaction = e.getAction();
		if(!gestureDetector.onTouchEvent(e))
		{
			super.onTouchEvent(e);
		}
		return(true);
	}
	
	//abstract method from onDoubleTapListener so must be implemented
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		System.out.println("SAM: "+"Double Tap");
		if(gbLight==true)
		{
			gbLight=false;
		}else
		{
			gbLight=true;
		}
		return(true);
	}
	
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		
		return(true);
	}
	
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		System.out.println("SAM: "+"Single Tap");
		if(gbSingleTap==true){
			gbSingleTap=false;
		}else{
			gbSingleTap=true;
		}
		return(true);
	}
	
	//abstract method from onGestureListener so must be implemented
	@Override
	public boolean onDown(MotionEvent e)
	{
			return(true);
	}
	
	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return(true);
	}
	
	@Override
	public void onLongPress(MotionEvent e)
	{
		System.out.println("SAM: "+"Long Press");
	}
	
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		System.out.println("SAM: "+"Scroll");
		uninitialize();
		System.exit(0);
		return(true);
	}
	
	@Override
	public void onShowPress(MotionEvent e)
	{
		
	}
	
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return(true);
	}
	
	private void initialize(GL10 gl)
	{
		//VERTEX SHADER
		//create shader
		gVertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		
		//source code
		vertexShaderSourceCodePerVertext = String.format("#version 320 es"+
											"\n"+
											"in vec4 vPosition;						"+
				"in vec3 vNormal;"+
				"uniform mat4 u_model_matrix;"+
				"uniform mat4 u_view_matrix;"+
				"uniform mat4 u_projection_matrix;"+
				"uniform int u_LKeyPressed;"+
				"uniform vec3 u_La;"+
				"uniform vec3 u_Ld;"+
				"uniform vec3 u_Ls;"+
				"uniform vec4 u_Lp;"+
				"uniform vec3 u_Ka;"+
				"uniform vec3 u_Kd;"+
				"uniform vec3 u_Ks;"+
				"uniform float u_KShininess;"+
				"out vec3 phong_ads_color;"+
				"void main(void)						"+
				"{										"+
				"if(u_LKeyPressed == 1)"+
				"{"+
				"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"+
				"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
				"vec3 light_direction = normalize(vec3(u_Lp) - eyeCoordinates.xyz);"+
				"float tn_dot_ld=max(dot(transformed_normals, light_direction),0.0);"+
				"vec3 ambient = u_La * u_Ka;"+
				"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
				"vec3 reflection_vector = reflect(-light_direction, transformed_normals);"+
				"vec3 viewer_vector = normalize(-eyeCoordinates.xyz);"+
				"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector),0.0), u_KShininess);"+
				"phong_ads_color=ambient+diffuse+specular;"+
				"}"+
				"else"+
				"{"+
				"phong_ads_color=vec3(1.0,1.0,1.0);"+
				"}"+
				"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;	"+
				"}										");
				
		vertexShaderSourceCodePerFragment = String.format(
					"#version 320 es"+
					"\n"+
					"in vec4 vPosition;						"+
					"in vec3 vNormal;"+
					"uniform mat4 u_model_matrix;"+
					"uniform mat4 u_view_matrix;"+
					"uniform mat4 u_projection_matrix;"+
					"uniform mediump int u_LKeyPressed;"+
					"uniform vec4 u_Lp;"+
					"out vec3 transformed_normals;"+
					"out vec3 light_direction;"+
					"out vec3 viewer_vector;"+
					"void main(void)						"+
					"{										"+
					"if(u_LKeyPressed == 1)"+
					"{"+
					"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"+
					"transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
					"light_direction = vec3(u_Lp) - eyeCoordinates.xyz;"+
					"viewer_vector = -eyeCoordinates.xyz;"+
					"}"+
					"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;	"+
					"}										");
		
		
		
		//FRAGMENT SHADER
		//create shader
		gFragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        // 2. Fragment Shader Source Code
		fragmentShaderSourceCodePerVertex = String.format(
                "#version 320 es"+
                        "\n"+
                        "precision highp float;"+
                        "in vec3 phong_ads_color;"+
                        "out vec4 FragColor;"+
                        "void main(void)"+
                        "{"+
                        "FragColor = vec4(phong_ads_color, 1.0);"+
                        "}"
        );
		
        fragmentShaderSourceCodePerFragment = String.format(
                "#version 320 es"+
                        "\n"+
                        "precision highp float;"+
                        "in vec3 transformed_normals;"+
                        "in vec3 light_direction;"+
                        "in vec3 viewer_vector;"+
                        "out vec4 FragColor;"+
                        "uniform vec3 u_La;"+
                        "uniform vec3 u_Ld;"+
                        "uniform vec3 u_Ls;"+
                        "uniform vec3 u_Ka;"+
                        "uniform vec3 u_Kd;"+
                        "uniform vec3 u_Ks;"+
                        "uniform float u_KShininess;"+
                        "uniform int u_LKeyPressed;"+
                        "void main(void)"+
                        "{"+
                        "vec3 phong_ads_color;"+
                        "if(u_LKeyPressed==1)"+
                        "{"+
                        "vec3 normalized_transformed_normals=normalize(transformed_normals);"+
                        "vec3 normalized_light_direction=normalize(light_direction);"+
                        "vec3 normalized_viewer_vector=normalize(viewer_vector);"+
                        "vec3 ambient = u_La * u_Ka;"+
                        "float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);"+
                        "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
                        "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);"+
                        "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_KShininess);"+
                        "phong_ads_color=ambient + diffuse + specular;"+
                        "}"+
                        "else"+
                        "{"+
                        "phong_ads_color = vec3(1.0, 1.0, 1.0);"+
                        "}"+
                        "FragColor = vec4(phong_ads_color, 1.0);"+
                        "}"
        );
		
		//sphere
		Sphere sphere=new Sphere();
        float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // vao
        GLES32.glGenVertexArrays(1,vao_sphere,0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // position vbo
        GLES32.glGenBuffers(1,vbo_sphere_position,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_VERTEX,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_VERTEX);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // normal vbo
        GLES32.glGenBuffers(1,vbo_sphere_normal,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // element vbo
        GLES32.glGenBuffers(1,vbo_sphere_element,0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES32.glBindVertexArray(0);
		
		// enable depth testing
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        // depth test to do
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        // We will always cull back faces for better performance
       // GLES32.glEnable(GLES32.GL_CULL_FACE);
        
        // Set the background color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		gbLight = false;
		Matrix.setIdentityM(gPerspectiveProjectionMatrix,0);
		
	}
	
	private void resize(int width, int height)
	{
		GLES32.glViewport(0,0,width,height);
        final float aspectRatio = width > height ? (float) width/(float) height : (float) height/(float)width;
        
        // perspective Projection => matrix,fov,aspect,near, far
        Matrix.perspectiveM(gPerspectiveProjectionMatrix,0,45.0f,aspectRatio,0.1f,100.0f);
    }

	public void display()
	{
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		//provide source code to shader
		if(gbSingleTap==true)//pv
		{
		GLES32.glShaderSource(gVertexShaderObject, vertexShaderSourceCodePerVertext);
		}else{//pf
			GLES32.glShaderSource(gVertexShaderObject, vertexShaderSourceCodePerFragment);
		}
		//compile source code
		GLES32.glCompileShader(gVertexShaderObject);
		int[] iShaderCompiledStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szInfoLog = null;
		GLES32.glGetShaderiv(gVertexShaderObject,GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus,0);
		
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gVertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
			if(iInfoLogLength[0]>0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(gVertexShaderObject);
				System.out.println("SAM: Vertex Shader Compilation Log: "+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		if(gbSingleTap==true)//pv
		{
		GLES32.glShaderSource(gFragmentShaderObject, fragmentShaderSourceCodePerVertex);
		}else{//pf
			GLES32.glShaderSource(gFragmentShaderObject, fragmentShaderSourceCodePerFragment);
		}
		GLES32.glCompileShader(gFragmentShaderObject);
		
		iShaderCompiledStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(gFragmentShaderObject,GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus,0);
		
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(gFragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
			if(iInfoLogLength[0]>0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(gFragmentShaderObject);
				System.out.println("SAM: Fragment Shader Compilation Log: "+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		//create shader program
		gShaderProgramObject = GLES32.glCreateProgram();
		GLES32.glAttachShader(gShaderProgramObject, gVertexShaderObject);
		GLES32.glAttachShader(gShaderProgramObject, gFragmentShaderObject);
		GLES32.glBindAttribLocation(gShaderProgramObject, GLESMacros.SAM_ATTRIBUTE_VERTEX,"vPosition");
        GLES32.glBindAttribLocation(gShaderProgramObject,GLESMacros.SAM_ATTRIBUTE_NORMAL,"vNormal");
		GLES32.glLinkProgram(gShaderProgramObject);
		
		int[] iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szInfoLog=null;
		GLES32.glGetProgramiv(gShaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(gShaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetProgramInfoLog(gShaderProgramObject);
                System.out.println("Shader Program Link Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
		
		
		//get MVP uniform location
	gModelMatrixUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	
	gLKeyPressedUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	
	gLaUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_La");
	gLdUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Ld");
	gLsUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Ls");
	gLpUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Lp");
	
	gKaUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Ka");
	gKdUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Kd");
	gKsUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Ks");
	gKShininessUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_KShininess");
		
		
		// use shader program
        GLES32.glUseProgram(gShaderProgramObject);
		
		if(gbLight==true)
		{
			GLES32.glUniform1i(gLKeyPressedUniform, 1);
			GLES32.glUniform3fv(gLaUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(gLdUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(gLsUniform, 1, lightSpecular, 0);
			GLES32.glUniform4fv(gLpUniform, 1, lightPosition,0);
			GLES32.glUniform3fv(gKaUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(gKdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(gKsUniform, 1, materialSpecular, 0);
			GLES32.glUniform1f(gKShininessUniform, materialShininess);
			
		}else
		{
			GLES32.glUniform1i(gLKeyPressedUniform, 0);
		}

        // OpenGL-ES drawing
        float modelMatrix[]=new float[16];
        float viewMatrix[]=new float[16];

        // set modelMatrix and viewMatrix matrices to identity matrix
        Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(viewMatrix,0);

        // apply z axis translation to go deep into the screen by -1.5,
        // so that pyramid with same fullscreen co-ordinates, but due to above translation will look small
        Matrix.translateM(modelMatrix,0,0.0f,0.0f,-1.5f);

        GLES32.glUniformMatrix4fv(gModelMatrixUniform,1,false,modelMatrix,0);
        GLES32.glUniformMatrix4fv(gViewMatrixUniform,1,false,viewMatrix,0);
        GLES32.glUniformMatrix4fv(gProjectionMatrixUniform,1,false,gPerspectiveProjectionMatrix,0);

        // bind vao
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES32.glBindVertexArray(0);
		
        
        // un-use shader program
        GLES32.glUseProgram(0);

        // render/flush
        requestRender();
	}
	
	void uninitialize()
    {
        // code
        
		
		// destroy vao
        if(vao_sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        
        // destroy position vbo
        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo
        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }
		
        if(gShaderProgramObject != 0)
        {
            if(gVertexShaderObject != 0)
            {
                // detach vertex shader from shader program object
                GLES32.glDetachShader(gShaderProgramObject, gVertexShaderObject);
                // delete vertex shader object
                GLES32.glDeleteShader(gVertexShaderObject);
                gVertexShaderObject = 0;
            }
            
            if(gFragmentShaderObject != 0)
            {
                // detach fragment  shader from shader program object
                GLES32.glDetachShader(gShaderProgramObject, gFragmentShaderObject);
                // delete fragment shader object
                GLES32.glDeleteShader(gFragmentShaderObject);
                gFragmentShaderObject = 0;
            }
        }

        // delete shader program object
        if(gShaderProgramObject != 0)
        {
            GLES32.glDeleteProgram(gShaderProgramObject);
            gShaderProgramObject = 0;
        }
    }
	
}