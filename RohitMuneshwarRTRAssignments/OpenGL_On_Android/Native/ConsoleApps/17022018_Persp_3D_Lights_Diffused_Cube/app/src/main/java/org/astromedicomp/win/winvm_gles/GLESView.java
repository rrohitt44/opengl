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

//a view for OpenGLES3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	
	private GestureDetector gestureDetector;
	
	private int gVertexShaderObject;
	private int gFragmentShaderObject;
	private int gShaderProgramObject;
	
	private int[] gVao_cube = new int[1];
	private int[] gVbo_cube_position = new int[1];
	private int[] gVbo_cube_normal = new int[1];

	
	private int gModelViewMatrixUniform, gProjectionMatrixUniform;
	private int gLdUniform, gLpUniform, gKdUniform;
	private int gLightingEnabledUniform;
	
	private float gAngleCube;
	
	private float[] gPerspectiveProjectionMatrix = new float[16];
	
	boolean gbAnimate, gbLight;
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
        System.out.println("VDG: OpenGL-ES Version = "+glesVersion);
        // get GLSL version
        String glslVersion=gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("VDG: GLSL Version = "+glslVersion);
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
		if(gbAnimate==true)
		{
			updateAngle();
		}
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
		System.out.println("VDG: "+"Double Tap");
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
		System.out.println("VDG: "+"Single Tap");
		if(gbAnimate==true)
		{
			gbAnimate=false;
		}else
		{
			gbAnimate=true;
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
		System.out.println("VDG: "+"Long Press");
	}
	
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		System.out.println("VDG: "+"Scroll");
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
		final String vertexShaderSourceCode = String.format("#version 320 es"+
											"\n"+
											"in vec4 vPosition;"+
											"in vec3 vNormal;"+
											"uniform mat4 u_mv_matrix;"+
											"uniform mat4 u_projection_matrix;"+
											"uniform mediump int u_lighting_enabled;"+
											"uniform vec3 u_Ld_uniform;"+
											"uniform vec3 u_Kd_uniform;"+
											"uniform vec4 u_Lp_uniform;"+
											"out vec3 out_diffuse_light;"+
											"void main(void)"+
											"{"+
											"if(u_lighting_enabled==1)"+
											"{"+
											"vec4 eyeCoordinates = u_mv_matrix * vPosition;"+
											"vec3 tnorm = normalize(mat3(u_mv_matrix) * vNormal);"+
											"vec3 s = normalize(vec3(u_Lp_uniform - eyeCoordinates));"+
											"out_diffuse_light = u_Ld_uniform * u_Kd_uniform * max(dot(s, tnorm),0.0);"+
											"}"+
											"gl_Position=u_projection_matrix * u_mv_matrix * vPosition;"+
											"}");
		
		//provide source code to shader
		GLES32.glShaderSource(gVertexShaderObject, vertexShaderSourceCode);
		
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
				System.out.println("VDG: Vertex Shader Compilation Log: "+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		//FRAGMENT SHADER
		//create shader
		gFragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		final String fragmentShaderSourceCode = String.format("#version 320 es"+
															"\n"+
															"precision highp float;"+
															"in vec3 out_diffuse_light;"+
															"uniform mediump int u_lighting_enabled;"+
															"out vec4 FragColor;"+
															"void main(void)"+
															"{"+
															"vec4 color;"+
															"if(u_lighting_enabled == 1)"+
															"{"+
															"color = vec4(out_diffuse_light, 1.0);"+
															"}"+
															"else"+
															"{"+
															"color = vec4(1.0,1.0,1.0,1.0);"+
															"}"+
															"FragColor = color;"+
															"}");
		GLES32.glShaderSource(gFragmentShaderObject, fragmentShaderSourceCode);
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
				System.out.println("VDG: Fragment Shader Compilation Log: "+szInfoLog);
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
		
		//get uniform locations
		gModelViewMatrixUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_mv_matrix");
		gProjectionMatrixUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
		gLightingEnabledUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");
		gLdUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Ld_uniform");
		gLpUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Lp_uniform");
		gKdUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_Kd_uniform");
		
		
		float[] cubeVertices = new float[]
		{
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,

            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,

            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
		};
	float[] cubeNormals = new float[]
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};
		
		
		//cube draw
		GLES32.glGenVertexArrays(1, gVao_cube, 0);
		GLES32.glBindVertexArray(gVao_cube[0]);
		
		//cube postion
		GLES32.glGenBuffers(1, gVbo_cube_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_cube_position[0]);
		
		ByteBuffer squareByteBuffer = ByteBuffer.allocateDirect(cubeVertices.length * 4);
		squareByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer squareVerticesBuffer = squareByteBuffer.asFloatBuffer();
		squareVerticesBuffer.put(cubeVertices);
		squareVerticesBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, squareVerticesBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_VERTEX,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_VERTEX);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		//cube normal
		GLES32.glGenBuffers(1, gVbo_cube_normal, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_cube_normal[0]);
		
		ByteBuffer squareColByteBuffer = ByteBuffer.allocateDirect(cubeNormals.length * 4);
		squareColByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer cubeNormalsBuffer = squareColByteBuffer.asFloatBuffer();
		cubeNormalsBuffer.put(cubeNormals);
		cubeNormalsBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeNormals.length * 4, cubeNormalsBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_NORMAL);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
        GLES32.glBindVertexArray(0);
		
		// enable depth testing
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        // depth test to do
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        // We will always cull back faces for better performance
       // GLES32.glEnable(GLES32.GL_CULL_FACE);
        
        // Set the background color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		gbAnimate = false;
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
		// use shader program
        GLES32.glUseProgram(gShaderProgramObject);
		
		if(gbLight==true)
		{
			GLES32.glUniform1i(gLightingEnabledUniform, 1);
			GLES32.glUniform3f(gLdUniform, 1.0f,1.0f,1.0f);
			GLES32.glUniform3f(gKdUniform, 0.5f,0.5f,0.5f);
				float[] lightPosition = {0.0f,0.0f,2.0f,1.0f};
			GLES32.glUniform4fv(gLpUniform, 1, lightPosition,0);
		}else
		{
			GLES32.glUniform1i(gLightingEnabledUniform, 0);
		}

        // OpenGL-ES drawing
        float modelMatrix[]=new float[16];
        float modelViewMatrix[]=new float[16];
        float rotationMatrix[]=new float[16];
		
        // CUBE CODE
        // set modelMatrix, modelViewMatrix, rotation matrices to identity matrix
        Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(modelViewMatrix,0);
        Matrix.setIdentityM(rotationMatrix,0);
		
		//translate by -3
        Matrix.translateM(modelMatrix,0,0.0f,0.0f,-5.0f);
		//rotate
        Matrix.setRotateM(rotationMatrix,0,gAngleCube,gAngleCube,gAngleCube,1.0f); // ALL axes rotation by angleCube angle
		
		float[] temp = new float[16];
        Matrix.multiplyMM(modelViewMatrix,0,modelMatrix,0,rotationMatrix,0);

        // pass above modelview matrix to the vertex shader in 'u_model_view_matrix' shader variable
        GLES32.glUniformMatrix4fv(gModelViewMatrixUniform,1,false,modelViewMatrix,0);
        // pass projection matrix to the vertex shader in 'u_projection_matrix' shader variable
        GLES32.glUniformMatrix4fv(gProjectionMatrixUniform,1,false,gPerspectiveProjectionMatrix,0);

        // bind vao
        GLES32.glBindVertexArray(gVao_cube[0]);
        
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);
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
        if(gVao_cube[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, gVao_cube, 0);
            gVao_cube[0]=0;
        }
        
        // destroy vbo
        if(gVbo_cube_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, gVbo_cube_position, 0);
            gVbo_cube_position[0]=0;
        }
		
		if(gVbo_cube_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, gVbo_cube_normal, 0);
            gVbo_cube_normal[0]=0;
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
	
	private void updateAngle(){
	if(gAngleCube>=360.0f)
	{
		gAngleCube=0.0f;
	}	
	
	gAngleCube=gAngleCube+0.5f;
}
}