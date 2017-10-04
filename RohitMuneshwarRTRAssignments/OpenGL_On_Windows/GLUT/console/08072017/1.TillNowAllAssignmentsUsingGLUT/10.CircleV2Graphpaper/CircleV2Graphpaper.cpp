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
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(-1.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glEnd();
	
	glBegin(GL_LINES);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,-1.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glEnd();
	
	glLineWidth(1.0f);
	for(float index=-1.0;index<=1.0;index = index+0.05){
	glBegin(GL_LINES);
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(-1.0f,index,0.0f);
	glVertex3f(1.0f,index,0.0f);
	glEnd();
	}
	
	for(float index=-1.0;index<=1.0;index = index+0.05){
	glBegin(GL_LINES);
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(index,-1.0f,0.0f);
	glVertex3f(index,1.0f,0.0f);
	glEnd();
	}
	
	const float PI = 3.141592f;
	//#define PI 3.1415926535898 //or look for 'M_PI' in 'math.h'

GLint circle_points = 100; //you can try using 1000 or 10000

glBegin(GL_LINE_LOOP); //GL_LINES will also do
glColor3f(1.0f,1.0f,0.0f);
    for(int i=0; i < circle_points; i++)

    {

       float angle = 2 * PI * i/circle_points;

        glVertex2f(cos(angle)/2, sin(angle)/2);

    }

glEnd();
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