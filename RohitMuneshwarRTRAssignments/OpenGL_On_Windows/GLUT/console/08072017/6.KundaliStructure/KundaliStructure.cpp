#include<GL/freeglut.h>
#define WIN_WIDTH 800
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
	
	void drawSquare(float,float,float);
	void drawInnerSquare(float,float,float);
	void drawDiagonal1(float,float,float);
	void drawDiagonal2(float,float,float);
	glClear(GL_COLOR_BUFFER_BIT);
	//#declare CornflowerBlue = color red 0.258824 green 0.258824 blue 0.435294
	drawSquare(0.258824f,0.258824f,0.435294f);
	drawInnerSquare(0.258824f,0.258824f,0.435294f);
	drawDiagonal1(0.258824f,0.258824f,0.435294f);
	drawDiagonal2(0.258824f,0.258824f,0.435294f);
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



void drawSquare(float red,float green, float blue){
	glBegin(GL_LINE_LOOP);
	glColor3f(red,green,blue);
	glVertex3f(-0.5f,0.5f,0.0f);
	glVertex3f(-0.5f,-0.5f,0.0f);
	glVertex3f(0.5f,-0.5f,0.0f);
	glVertex3f(0.5f,0.5f,0.0f);
	glEnd();
}

void drawInnerSquare(float red,float green, float blue){
	glBegin(GL_LINE_LOOP);
	glColor3f(red,green,blue);
	glVertex3f(0.0f,0.5f,0.0f);
	glVertex3f(-0.5f,0.0f,0.0f);
	glVertex3f(0.0f,-0.5f,0.0f);
	glVertex3f(0.5f,0.0f,0.0f);
	glEnd();
}

void drawDiagonal1(float red,float green, float blue){
	glBegin(GL_LINES);
	glColor3f(red,green,blue);
	glVertex3f(-0.5f,0.5f,0.0f);
	glVertex3f(0.5f,-0.5f,0.0f);
	glEnd();
}

void drawDiagonal2(float red,float green, float blue){
	glBegin(GL_LINES);
	glColor3f(red,green,blue);
	glVertex3f(0.5f,0.5f,0.0f);
	glVertex3f(-0.5f,-0.5f,0.0f);
	glEnd();
}


