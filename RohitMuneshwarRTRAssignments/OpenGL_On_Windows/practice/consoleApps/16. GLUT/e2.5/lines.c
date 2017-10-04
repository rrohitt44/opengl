#include<GL/glut.h>
#include<stdlib.h>

#define drawOneLine(x1,y1,x2,y2) glBegin(GL_LINES); \
	glVertex2f(x1,y1); glVertex2f(x2,y2); glEnd();
	
void init(void){
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glShadeModel(GL_FLAT);
}

void display(void){
	int i;
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_LINE_STIPPLE);
	
	glLineStipple(1,0x0101);
	drawOneLine(50.0f,125.0f,150.0f,125.0f);
	glLineStipple(1,0x00FF);
	drawOneLine(150.0f,125.0f,250.0f,125.0f);
	glLineStipple(1,0x1C47);
	drawOneLine(250.0f,125.0f,350.0f,125.0f);
	
	glLineWidth(5.0f);
	glLineStipple(1,0x0101);
	drawOneLine(50.0f,100.0f,150.0f,100.0f);
	glLineStipple(1,0x00FF);
	drawOneLine(150.0f,100.0f,250.0f,100.0f);
	glLineStipple(1,0x1C47);
	drawOneLine(250.0f,100.0f,350.0f,100.0f);
	glLineWidth(1.0f);
	
	glLineStipple(1,0x1C47);
	glBegin(GL_LINE_STRIP);
	for(i=0;i<7;i++){
		glVertex2f(50.0f + ((GLfloat) i * 50.0f),75.0f);
	}
	glEnd();
	
	for(i=0;i<6;i++){
		drawOneLine(50.0f + ((GLfloat) i * 50.0f), 50.0f, 
					50.0f + ((GLfloat) (i+1) * 50.0f), 50.0f);
	}
	
	glLineStipple(5, 0x1C47);
	drawOneLine(50.0f,25.0f,350.0f,25.0f);
	
	
	glDisable(GL_LINE_STIPPLE);
	glFlush();
}

void reshape(int w,int h){
	glViewport(0,0,(GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f,(GLdouble) w,0.0f, (GLdouble) h);
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400,150);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}