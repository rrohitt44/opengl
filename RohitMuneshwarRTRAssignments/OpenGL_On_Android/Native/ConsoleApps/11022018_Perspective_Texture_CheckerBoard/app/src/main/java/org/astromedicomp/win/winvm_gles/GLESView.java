package org.astromedicomp.win.winvm_gles;
import android.content.Context; // for drawing context related
import android.opengl.GLSurfaceView; // for OpenGL Surface View and all related
import javax.microedition.khronos.opengles.GL10; // for OpenGLES 1.0 needed as param type GL10
import javax.microedition.khronos.egl.EGLConfig; // for EGLConfig needed as param type EGLConfig
import android.opengl.GLES32; // for OpenGLES 3.2
import android.view.MotionEvent; // for "MotionEvent"
import android.view.GestureDetector; // for GestureDetector
import android.view.GestureDetector.OnGestureListener; // OnGestureListener
import android.view.GestureDetector.OnDoubleTapListener; // for OnDoubleTapListener

// for vbo
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.Matrix; // for Matrix math    


// A view for OpenGLES3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {
    private final Context context;

    private GestureDetector gestureDetector;

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;

    private int[] gVao_StraightBoard = new int[1];
    private int[] gVbo_StraightBoard_Position = new int[1];

    private int[] gVao_TiltedBoard = new int[1];
    private int[] gVbo_TiltedBoard_Position = new int[1];

    private int[] gVbo_CheckerBoard_Texture = new int[1];

    private int mvpUniform;
    private int texture0_sampler_uniform;

    private int[] texture_checkerBoard = new int[1];


    private float perspectiveProjectionMatrix[] = new float[16]; // 4x4 matrix

    public GLESView(Context drawingContext) {
        super(drawingContext);

        context = drawingContext;

        // accordingly set EGLContext to current supported version of OpenGL-ES
        setEGLContextClientVersion(3);

        // set Renderer for drawing on the GLSurfaceView
        setRenderer(this);

        // Render the view only when there is a change in the drawing data
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        gestureDetector = new GestureDetector(context, this, null, false); // this means 'handler' i.e. who is going to handle
        gestureDetector.setOnDoubleTapListener(this); // this means 'handler' i.e. who is going to handle
    }

    // overriden method of GLSurfaceView.Renderer ( Init Code )
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        // get OpenGL-ES version
        String glesVersion = gl.glGetString(GL10.GL_VERSION);
        System.out.println("SAM: OpenGL-ES Version = " + glesVersion);
        // get GLSL version
        String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("SAM: GLSL Version = " + glslVersion);

        initialize(gl);

    }

    // overriden method of GLSurfaceView.Renderer ( Chnge Size Code )
    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        resize(gl10, width, height);
    }

    // overriden method of GLSurfaceView.Renderer ( Rendering Code )
    @Override
    public void onDrawFrame(GL10 gl10) {
        display(gl10);
    }

    // Handling 'onTouchEvent' Is The Most IMPORTANT,
    // Because It Triggers All Gesture And Tap Events
    @Override
    public boolean onTouchEvent(MotionEvent e) {
        // code
        int eventaction = e.getAction();
        if (!gestureDetector.onTouchEvent(e))
            super.onTouchEvent(e);
        return (true);
    }

    // abstract method from OnDoubleTapListener so must be implemented
    @Override
    public boolean onDoubleTap(MotionEvent e) {
        return (true);
    }

    // abstract method from OnDoubleTapListener so must be implemented
    @Override
    public boolean onDoubleTapEvent(MotionEvent e) {
        // Do Not Write Any code Here Because Already Written 'onDoubleTap'
        return (true);
    }

    // abstract method from OnDoubleTapListener so must be implemented
    @Override
    public boolean onSingleTapConfirmed(MotionEvent e) {
        return (true);
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onDown(MotionEvent e) {
        // Do Not Write Any code Here Because Already Written 'onSingleTapConfirmed'
        return (true);
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        return (true);
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public void onLongPress(MotionEvent e) {
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        uninitialize();
        System.exit(0);
        return (true);
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public void onShowPress(MotionEvent e) {
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onSingleTapUp(MotionEvent e) {
        return (true);
    }

    private void initialize(GL10 gl) {
        // ***********************************************
        // Vertex Shader
        // ***********************************************
        // create shader
        vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        // vertex shader source code
        final String vertexShaderSourceCode = String.format
                (
                        "#version 320 es" +
                                "\n" +
                                "in vec4 vPosition;" +
                                "in vec2 vTexture0_Coord;" +
                                "out vec2 out_texture0_coord;" +
                                "uniform mat4 u_mvp_matrix;" +
                                "void main(void)" +
                                "{" +
                                "gl_Position = u_mvp_matrix * vPosition;" +
                                "out_texture0_coord = vTexture0_Coord;" +
                                "}"
                );

        // provide source code to shader
        GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

        // compile shader & check for errors
        GLES32.glCompileShader(vertexShaderObject);
        int[] iShaderCompiledStatus = new int[1];
        int[] iInfoLogLength = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if (iShaderCompiledStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("SAM: Vertex Shader Compilation Log = " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // ***********************************************
        // Fragment Shader
        // ***********************************************
        // create shader
        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        // fragment shader source code
        final String fragmentShaderSourceCode = String.format
                (
                        "#version 320 es" +
                                "\n" +
                                "precision highp float;" +
                                "in vec2 out_texture0_coord;" +
                                "uniform highp sampler2D u_texture0_sampler;" +
                                "out vec4 FragColor;" +
                                "void main(void)" +
                                "{" +
                                "FragColor = texture(u_texture0_sampler, out_texture0_coord);" +
                                "}"
                );

        // provide source code to shader
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);

        // compile shader and check for errors
        GLES32.glCompileShader(fragmentShaderObject);
        iShaderCompiledStatus[0] = 0; // re-initialize
        iInfoLogLength[0] = 0; // re-initialize
        szInfoLog = null; // re-initialize
        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if (iShaderCompiledStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("SAM: Fragment Shader Compilation Log = " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // create shader program
        shaderProgramObject = GLES32.glCreateProgram();

        // attach vertex shader to shader program
        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);

        // attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

        // pre-link binding of shader program object with vertex shader attributes
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SAM_ATTRIBUTE_VERTEX, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SAM_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");

        // link the two shaders together to shader program object
        GLES32.glLinkProgram(shaderProgramObject);
        int[] iShaderProgramLinkStatus = new int[1];
        iInfoLogLength[0] = 0; // re-initialize
        szInfoLog = null; // re-initialize
        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("SAM: Shader Program Link Log = " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // get MVP uniform location
        mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
        texture0_sampler_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_texture0_sampler");

        texture_checkerBoard[0] = loadGLTexture();

        // *** vertices, colors, shader attribs, vbo, vao initializations ***

        final float straightFacingQuadVertices [] = {
                -2.0f,-1.0f,0.0f,
                -2.0f,1.0f,0.0f,
                0.0f,1.0f,0.0f,
                0.0f,-1.0f,0.0f
        };

        final float tiltedFacingQuadVertices [] = {
                1.0f,-1.0f,0.0f,
                1.0f,1.0f,0.0f,
                2.41421f,1.0f,-1.41421f,
                2.41421f,-1.0f,-1.41421f
        };
        final float  checkerBoardTexcoords[] =
                {
                        0.0f, 0.0f,
                        0.0f,1.0f,
                        1.0f,1.0f,
                        1.0f,0.0f
                };


        // Triangle
        GLES32.glGenVertexArrays(1, gVao_StraightBoard, 0);
        GLES32.glBindVertexArray(gVao_StraightBoard[0]);

        GLES32.glGenBuffers(1, gVbo_StraightBoard_Position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_StraightBoard_Position[0]);

        ByteBuffer byteBufferT = ByteBuffer.allocateDirect(straightFacingQuadVertices.length * 4);
        byteBufferT.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBufferT = byteBufferT.asFloatBuffer();
        verticesBufferT.put(straightFacingQuadVertices);
        verticesBufferT.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                straightFacingQuadVertices.length * 4,
                verticesBufferT,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_VERTEX,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_VERTEX);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


        GLES32.glGenBuffers(1, gVbo_CheckerBoard_Texture, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_CheckerBoard_Texture[0]);

        ByteBuffer byteBufferColor = ByteBuffer.allocateDirect(checkerBoardTexcoords.length * 4);
        byteBufferColor.order(ByteOrder.nativeOrder());
        FloatBuffer colorsBuffer = byteBufferColor.asFloatBuffer();
        colorsBuffer.put(checkerBoardTexcoords);
        colorsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                checkerBoardTexcoords.length * 4,
                colorsBuffer,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_TEXTURE0,
                2,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_TEXTURE0);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        // GLES32.glVertexAttrib3f(GLESMacros.SAM_ATTRIBUTE_COLOR,1.0f, 1.0f, 1.0f);
        GLES32.glBindVertexArray(0);

        // Square
        GLES32.glGenVertexArrays(1, gVao_TiltedBoard, 0);
        GLES32.glBindVertexArray(gVao_TiltedBoard[0]);

        GLES32.glGenBuffers(1, gVbo_TiltedBoard_Position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_TiltedBoard_Position[0]);

        ByteBuffer byteBufferS = ByteBuffer.allocateDirect(tiltedFacingQuadVertices.length * 4);
        byteBufferS.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBufferS = byteBufferS.asFloatBuffer();
        verticesBufferS.put(tiltedFacingQuadVertices);
        verticesBufferS.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                tiltedFacingQuadVertices.length * 4,
                verticesBufferS,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_VERTEX,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_VERTEX);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, gVbo_CheckerBoard_Texture, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_CheckerBoard_Texture[0]);

        ByteBuffer byteBufferColorP = ByteBuffer.allocateDirect(checkerBoardTexcoords.length * 4);
        byteBufferColorP.order(ByteOrder.nativeOrder());
        FloatBuffer colorsBufferP = byteBufferColorP.asFloatBuffer();
        colorsBufferP.put(checkerBoardTexcoords);
        colorsBufferP.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                checkerBoardTexcoords.length * 4,
                colorsBufferP,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.SAM_ATTRIBUTE_TEXTURE0,
                2,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.SAM_ATTRIBUTE_TEXTURE0);


        GLES32.glBindVertexArray(0);


        // enable depth testing
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        // depth test to do
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        // We will always cull back faces for better performance
        //GLES32.glEnable(GLES32.GL_CULL_FACE);

        // Set the background color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // set projectionMatrix to identitu matrix
        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private int loadGLTexture() {
        // code

        int[] texture = new int[1];

        // create a texture object to apply to model
        GLES32.glGenTextures(1, texture, 0);

        // indicate that pixel rows are tightly packed (defaults to stride of 4 which is kind of only good for RGBA or FLOAT data types)
        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);

        // bind with the texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);

        // set up filter and wrap modes for this texture object
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,GLES32.GL_TEXTURE_WRAP_S,GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,GLES32.GL_TEXTURE_WRAP_T,GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);

        // load the bitmap into the bound texture
     //   GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
        int  c=0;
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(64 * 64 *  4);
        byteBuffer.order(ByteOrder.nativeOrder());
        for(int i=0;i<64;i++){
            for(int j=0;j<64;j++){
                for(int k=0;k<4;k++) {
                    c = ((i & 0x8) ^ (j & 0x8)) * 255;
                    if(k==3)
                        byteBuffer.put((byte)255);
                    else
                        byteBuffer.put((byte)c);
                }
            }
        }
        byteBuffer.position(0);
        GLES32.glTexImage2D(GLES32.GL_TEXTURE_2D,0,GLES32.GL_RGBA,64,64,0,GLES32.GL_RGBA,GLES32.GL_UNSIGNED_BYTE,byteBuffer);
        // generate mipmap
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);

        return (texture[0]);
    }

    private void resize(GL10 gl10, int width, int height) {
        // Adjust the viewport based on geometry changes,
        // such as screen rotation
        GLES32.glViewport(0, 0, width, height);

        final float aspectRatio = width > height ? (float) width / (float) height : (float) height / (float) width;

        // perspective Projection => matrix,fov,aspect,near, far
        perspectiveM(perspectiveProjectionMatrix, 45, aspectRatio, 1.0f, 10.0f);

    }

    public static void perspectiveM(float m[], float yFovInDegrees, float aspect, float n, float f) {
        final float angleInRadian = (float) (yFovInDegrees * Math.PI / 180);
        final float a = (float) (1.0f / Math.tan(angleInRadian / 2.0f));
        m[0] = a / aspect;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;

        m[4] = 0.0f;
        m[5] = a;
        m[6] = 0.0f;
        m[7] = 0.0f;

        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = -((f + n) / (f - n));
        m[11] = -1.0f;

        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = -((2.0f * f * n) / (f - n));
        m[15] = 0.0f;

    }

    public void display(GL10 gl10) {
        // code
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        // use shader program
        GLES32.glUseProgram(shaderProgramObject);

        float modelViewMatrix[] = new float[16];
        float modelViewProjectionMatrix[] = new float[16];
      //  float rotationMatrix[] = new float[16];
        // Triangle OpenGL-ES drawing

        // set modelview & modelviewprojection matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
      //  Matrix.setIdentityM(rotationMatrix, 0);

        Matrix.translateM(modelViewMatrix, 0, -1.0f, 0.0f, -5.0f);
        //  Matrix.rotateM(rotationMatrix, 0, gAnglePyramid, 0.0f, 1.0f, 0.0f);

      //  final float[] temp = new float[16];
      //  Matrix.multiplyMM(temp, 0, modelViewMatrix, 0, rotationMatrix, 0);
        // multiply the modelview and projection matrix to get modelviewprojection matrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        // pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
        // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // bind vao
        GLES32.glBindVertexArray(gVao_StraightBoard[0]);


        //bind pyramid texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_checkerBoard[0]);
        //othe sampler enable as we have only one texture sampler in fragment shader
        GLES32.glUniform1i(texture0_sampler_uniform, 0);

        // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        // unbind vao
        GLES32.glBindVertexArray(0);


        // Square
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
       // Matrix.setIdentityM(rotationMatrix, 0);

        Matrix.translateM(modelViewMatrix, 0, 1.0f, 0.0f, -5.0f);
      //  Matrix.rotateM(rotationMatrix, 0, gAngleCube, 1.0f, 1.0f, 1.0f);
       // final float[] temp1 = new float[16];
      //  Matrix.multiplyMM(temp1, 0, modelViewMatrix, 0, rotationMatrix, 0);

        // multiply the modelview and projection matrix to get modelviewprojection matrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        // pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
        // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // bind vao
        GLES32.glBindVertexArray(gVao_TiltedBoard[0]);

        //bind cube texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_checkerBoard[0]);
        //othe sampler enable as we have only one texture sampler in fragment shader
        GLES32.glUniform1i(texture0_sampler_uniform, 0);


        // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        // unbind vao
        GLES32.glBindVertexArray(0);

        // un-use shader program
        GLES32.glUseProgram(0);

        // render/flush
        requestRender();
    }



    void uninitialize() {
        // code
        // destroy vao


        if (shaderProgramObject != 0) {
            if (vertexShaderObject != 0) {
                // detach vertex shader from shader program object
                GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
                // delete vertex shader object
                GLES32.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }

            if (fragmentShaderObject != 0) {
                // detach fragment  shader from shader program object
                GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
                // delete fragment shader object
                GLES32.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }
        }

        // delete shader program object
        if (shaderProgramObject != 0) {
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }
    }
}
