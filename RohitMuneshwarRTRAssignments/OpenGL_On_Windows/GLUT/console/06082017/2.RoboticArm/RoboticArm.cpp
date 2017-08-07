#include<GL/freeglut.h>
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
bool gbFullScreen=false;
int shoulder = 0, elbow = 0;
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
	if(y == 0){
		y = 1;
	}
	glViewport(0,0,(GLsizei) x, (GLsizei) y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat) x / (GLfloat) y, 0.1f, 100.0f);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0.0f,0.0f,5.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	glColor3f(1.0f,0.0f,0.0f);
	
	glPushMatrix();
	glTranslatef(-1.0f,0.0f,0.0f);
	glRotatef((GLfloat) shoulder, 0.0f,0.0f,1.0f);
	glTranslatef(1.0f,0.0f,0.0f);
	glPushMatrix();
	glScalef(2.0f,0.4f,1.0f);
	glutWireCube(1.0);
	glPopMatrix();
	
	glTranslatef(1.0f,0.0f,0.0f);
	glRotatef((GLfloat) elbow, 0.0f,0.0f,1.0f);
	glTranslatef(1.0f,0.0f,0.0f);
	glPushMatrix();
	glScalef(2.0f,0.4f,1.0f);
	glutWireCube(1.0);
	glPopMatrix();
	
	glPopMatrix();
	glutSwapBuffers();
}

void initialize(){
	glClearColor(0.0f,0.0f,0.0f,0.0f);
}

void keyboard(unsigned char key, int x, int y){
	switch(key){
		
		case 's':
			shoulder = (shoulder + 10)%360;
			glutPostRedisplay();
		break;
		
		case 'S':
			shoulder = (shoulder - 10)%360;
			glutPostRedisplay();
		break;
		
		case 'e':
			elbow = (elbow+5)%360;
			glutPostRedisplay();
		break;
		
		case 'E':
			elbow = (elbow - 5)%360;
			glutPostRedisplay();
		break;
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