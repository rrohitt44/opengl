#include<GL/freeglut.h>
#include<math.h>
#define WIN_WIDTH 600
#define WIN_HEIGHT 600
bool gbFullScreen=false;
int main(int argc, char** argv){
	void uninitialize(void);
	void resize(int,int);
	void display(void);
	
	void initialize(void);
	void keyboard(unsigned char, int, int);
	void mouse(int,int,int,int);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //GLUT_SINGLE
	glutInitWindowSize(WIN_WIDTH,WIN_HEIGHT);
	glutInitWindowPosition(100,100);
	glutCreateWindow("OpenGL First Window - using freeglut");
	initialize();
	
	glutDisplayFunc(display);
	
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);
	glutMainLoop();
}

void uninitialize(){
	//code here
}

void resize(int x,int y){
	//code here
	glViewport(0,0,(GLsizei) x, (GLsizei) y);
}

void display(){
	void drawCircle(float,float,float,float);
	glClear(GL_COLOR_BUFFER_BIT);
	//#declare CornflowerBlue = color red 0.258824 green 0.258824 blue 0.435294
	//for(float i=1;i<2;i=i+0.1){
		//float i = 1.5;
		//drawGrid();
		for(float i=0.1f;i<=1.0f;i=i+0.1f){
			drawCircle(i,0.258824f,0.258824f,0.435294f);
			//drawCircle(2,0.258824f,0.258824f,0.435294f);
		}
	//}
	glutSwapBuffers();
}

void initialize(){
	glClearColor(0.0f,0.0f,0.0f,0.0f);
}

void keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:
		glutLeaveMainLoop();
		break;
		
		case 'f':
		case 'F':
		if(gbFullScreen == false){
		glutFullScreen();
		gbFullScreen = true;
		}else{
			glutLeaveFullScreen();
			gbFullScreen = false;
		}
		break;
		
		default:
		break;
	}
}

void mouse(int button, int state, int x,int y){
	switch(button){
		case GLUT_LEFT_BUTTON:
		break;
		default:
		break;
	}
}

void drawCircle(float i, float red,float green, float blue){
	const float PI = 3.141592f;
	GLint circle_points = 10000;
	glBegin(GL_POINTS);
	if(i==0.1f){
		glColor3f(1.0f,0.0f,0.0f);
	}else if(i==0.2f){
		glColor3f(0.0f,1.0f,0.0f);
	}else if(i==0.3f){
		glColor3f(0.0f,0.0f,1.0f);
	}else if(i==0.4f){
		glColor3f(1.0f,1.0f,0.0f);
	}else if(i==0.5f){
		glColor3f(1.0f,0.0f,1.0f);
	}else if(i==0.6f){
		glColor3f(0.0f,1.0f,1.0f);
	}else if(i>0.6f){
		glColor3f(i,i,i);
	}
	//for(float angle=0.0f;angle<2.0f * PI; angle = angle+ 0.01f){
	for(int iq=0;iq<circle_points;iq++){
		float angle = 2 * PI * iq / circle_points;
		glVertex3f(cos(angle)*i, sin(angle)*i, 0.0f);
	}
	glEnd();
}
