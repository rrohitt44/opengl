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
	void drawPoint(float size, float x1, float y1, float red, float green, float blue);
	glClear(GL_COLOR_BUFFER_BIT);
	
	drawPoint(5.0f,0.0f,0.0f,1.0f,1.0f,1.0f);
	
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

void drawPoint(float size, float x1, float y1, float red, float green, float blue){
	glPointSize(size);
	glBegin(GL_POINTS);
	glColor3f(red, green, blue);
	glVertex3f(x1,y1, 0.0f);
	glEnd();
}