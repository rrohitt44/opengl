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
	void drawDeathlyHollow(float,float,float);
	glClear(GL_COLOR_BUFFER_BIT);
	
			drawDeathlyHollow(0.258824f,0.258824f,0.435294f);
		
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

void drawDeathlyHollow(float red,float green, float blue){
	const float PI = 3.141592f;
	GLint circle_points = 1000;
	
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(0.0f,0.5f,0.0f);
	glVertex3f(0.0f,-0.5f,0.0f);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(0.0f,0.5f,0.0f);
	glVertex3f(-0.5f,-0.5f,0.0f);
	glVertex3f(0.5f,-0.5f,0.0f);
	glEnd();
	
	float dAB = sqrt(((0.0f-(-0.5f))*(0.0f-(-0.5f))) + ((0.5f-(-0.5f))*(0.5f-(-0.5f))));
	float dBC = sqrt(((-0.5f-0.5f)*(-0.5f-0.5f))+((-0.5f+0.5f)*(-0.5f+0.5f)));
	float dCA = sqrt((((0.0f)-(0.5f))*((0.0f)-(0.5f))) + (((0.5f)-(-0.5f))*((0.5f)-(-0.5f))));
	
	float perimeter = dAB + dBC + dCA;
	float semiperimeter = perimeter/2;
	
	float ix = ((dBC*(0.0f)) + (dCA * (-0.5f)) + (dAB * (0.5f)))/perimeter;
	float iy = ((dBC*(0.5f)) + (dCA * (-0.5f)) + (dAB * (-0.5f)))/perimeter;
	
	float radius = (sqrt(semiperimeter*(semiperimeter-dAB)*(semiperimeter-dBC)*(semiperimeter-dCA)))/semiperimeter;
	
	glBegin(GL_POINTS);
	//for(float angle=0.0f;angle<2.0f * PI; angle = angle+ 0.01f){
	for(int iq=0;iq<circle_points;iq++){
		float angle = 2 * PI * iq / circle_points;
		glVertex3f((radius*(cos(angle))+ix), radius*(sin(angle))+iy, 0.0f);
	}
	glEnd();
}
