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

import android.graphics.BitmapFactory; // texture factory
import android.graphics.Bitmap; // for PNG image
import android.opengl.GLUtils; // for texImage2D()

import org.astromedicomp.win.persp_2d_tex_anim_geo.R;


//a view for OpenGLES3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	
	private GestureDetector gestureDetector;
	
	private int gVertexShaderObject;
	private int gFragmentShaderObject;
	private int gShaderProgramObject;
	
	private int[] gVao_square = new int[1];
	private int[] gVbo_square_position = new int[1];
	private int[] gVbo_square_texture = new int[1];
	
	
	private int gMVPUniform;
	private int texture0_sampler_uniform;
	
	private int[] texture_smiley = new int[1];
	
	private float[] gPerspectiveProjectionMatrix = new float[16];
	
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
											"in vec2 vTexture0_Coord;"+
											"uniform mat4 u_mvp_matrix;"+
											"out vec2 out_texture0_coord;"+
											"void main(void)"+
											"{"+
											"gl_Position=u_mvp_matrix * vPosition;"+
											"out_texture0_coord = vTexture0_Coord;"+
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
															"in vec2 out_texture0_coord;"+
															"uniform highp sampler2D u_texture0_sampler;"+
															"out vec4 FragColor;"+
															"void main(void)"+
															"{"+
															"FragColor = texture(u_texture0_sampler, out_texture0_coord);"+
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
        GLES32.glBindAttribLocation(gShaderProgramObject,GLESMacros.SAM_ATTRIBUTE_TEXTURE0,"vTexture0_Coord");
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
		
		gMVPUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
		
		// get texture sampler uniform location
        texture0_sampler_uniform=GLES32.glGetUniformLocation(gShaderProgramObject,"u_texture0_sampler");
		
		// load textures
        texture_smiley[0]=loadGLTexture(R.raw.smiley);
        
		 // *** vertices, colors, shader attribs, vbo, vao initializations ***
        final float squareVertices[] =
                {
                        1.0f,1.0f,0.0f,
                        -1.0f,1.0f,0.0f,
                        -1.0f,-1.0f,0.0f,
                        1.0f,-1.0f,0.0f
                };

        final float squareTexcoords[]=
                {
                        1.0f,1.0f,
                        0.0f,1.0f,
                        0.0f,0.0f,
                        1.0f,0.0f

                };
		
		//triangle draw
		GLES32.glGenVertexArrays(1, gVao_square, 0);
		GLES32.glBindVertexArray(gVao_square[0]);
		
		//triangle position
		GLES32.glGenBuffers(1, gVbo_square_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_square_position[0]);
		
		ByteBuffer triPosByteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
		triPosByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer triPosVerticesBuffer = triPosByteBuffer.asFloatBuffer();
		triPosVerticesBuffer.put(squareVertices);
		triPosVerticesBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareVertices.length * 4, triPosVerticesBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_VERTEX,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_VERTEX);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		
		GLES32.glGenBuffers(1, gVbo_square_texture, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_square_texture[0]);
		
		ByteBuffer triColByteBuffer = ByteBuffer.allocateDirect(squareTexcoords.length * 4);
		triColByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer triColVerticesBuffer = triColByteBuffer.asFloatBuffer();
		triColVerticesBuffer.put(squareTexcoords);
		triColVerticesBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareTexcoords.length * 4, triColVerticesBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_TEXTURE0,
                                     2,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_TEXTURE0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        GLES32.glBindVertexArray(0);
		
		
       // GLES32.glBindVertexArray(0);
		
		// enable depth testing
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        // depth test to do
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        // We will always cull back faces for better performance
        GLES32.glEnable(GLES32.GL_CULL_FACE);
        
        // Set the background color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		Matrix.setIdentityM(gPerspectiveProjectionMatrix,0);
		
	}
	
	
	private int loadGLTexture(int imageFileResourceID)
    {
        // code
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        
        // read in the resource
        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageFileResourceID, options);
        
        int[] texture=new int[1];
        
        // create a texture object to apply to model
        GLES32.glGenTextures(1, texture, 0);
        
        // indicate that pixel rows are tightly packed (defaults to stride of 4 which is kind of only good for RGBA or FLOAT data types)
        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT,1);
        
        // bind with the texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,texture[0]);
        
        // set up filter and wrap modes for this texture object
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,GLES32.GL_TEXTURE_MAG_FILTER,GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,GLES32.GL_TEXTURE_MIN_FILTER,GLES32.GL_LINEAR_MIPMAP_LINEAR);
        
        // load the bitmap into the bound texture
        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
        
        // generate mipmap
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
        
        return(texture[0]);
    }
	
	private void resize(int width, int height)
	{
		GLES32.glViewport(0,0,width,height);
        final float aspectRatio = width > height ? (float) width/(float) height : (float) height/(float)width;
        
        // perspective Projection => matrix,fov,aspect,near, far
        Matrix.perspectiveM(gPerspectiveProjectionMatrix,0,45.0f,aspectRatio,0.1f,100.0f);
    }

    /*public static void perspectiveM(float m[],float yFovInDegrees,float aspect,float n, float f){
        final float angleInRadian = (float) (yFovInDegrees* Math.PI/180);
        final float a = (float) (1.0f / Math.tan(angleInRadian/2.0f));
        m[0] = a/aspect;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;

        m[4] = 0.0f;
        m[5] = a;
        m[6] = 0.0f;
        m[7] = 0.0f;

        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = - ((f+n) / (f-n) );
        m[11] = -1.0f;

        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = -( (2.0f*f*n)/ (f-n) );
        m[15] = 0.0f;

    }*/
	public void display()
	{
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		// use shader program
        GLES32.glUseProgram(gShaderProgramObject);

        // OpenGL-ES drawing
        float modelViewMatrix[]=new float[16];
        float modelViewProjectionMatrix[]=new float[16];
		
		//triangle drawing
        // set modelview & modelviewprojection matrices to identity
        Matrix.setIdentityM(modelViewMatrix,0);
        Matrix.setIdentityM(modelViewProjectionMatrix,0);
		//translate by -3
		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);
		
        // multiply the modelview and projection matrix to get modelviewprojection matrix
        Matrix.multiplyMM(modelViewProjectionMatrix,0,gPerspectiveProjectionMatrix,0,modelViewMatrix,0);
        
        // pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
        // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
        GLES32.glUniformMatrix4fv(gMVPUniform,1,false,modelViewProjectionMatrix,0);
        
        // bind vao
        GLES32.glBindVertexArray(gVao_square[0]);
		
		 // bind with pyramid texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,texture_smiley[0]);
         // 0th sampler enable ( as we have only 1 texture sampler in fragment shader )
        GLES32.glUniform1i(texture0_sampler_uniform, 0);
        
        
		
		 // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,0,12); // 3 (each with its x,y,z ) vertices in triangleVertices array
        
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
        if(gVao_square[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, gVao_square, 0);
            gVao_square[0]=0;
        }
        
        // destroy vbo
        if(gVbo_square_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, gVbo_square_position, 0);
            gVbo_square_position[0]=0;
        }
		
		// destroy texture vbo
        if(gVbo_square_texture[0] != 0)
        {
            GLES32.glDeleteBuffers(1, gVbo_square_texture, 0);
            gVbo_square_texture[0]=0;
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
		
		// delete texture objects
        if (texture_smiley[0] != 0)
        {
            GLES32.glDeleteTextures(1, texture_smiley, 0);
            texture_smiley[0] = 0;
        }
    }
	
}