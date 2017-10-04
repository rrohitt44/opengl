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
	void drawGrid(void);
	void drawConcentricTriangles(float,float,float);
	glClear(GL_COLOR_BUFFER_BIT);
	//#declare CornflowerBlue = color red 0.258824 green 0.258824 blue 0.435294
	//for(float i=1;i<2;i=i+0.1){
		//float i = 1.5;
		//drawGrid();
	drawConcentricTriangles(0.258824f,0.258824f,0.435294f);
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

void drawLine(float width,float x1, float y1, float x2, float y2,float red,float green,float blue){
	glLineWidth(width);
	glBegin(GL_LINES);
	glColor3f(red,green,blue);
	glVertex3f(x1,y1,0.0f);
	glVertex3f(x2,y2,0.0f);
	glEnd();
}

void drawGrid(){
	void drawLine(float,float,float,float,float,float,float,float);
	
	drawLine(3.0f,-1.0f,0.0f,1.0f,0.0f,1.0f,0.0f,0.0f);
	drawLine(3.0f,0.0f,-1.0f,0.0f,1.0f,0.0f,1.0f,0.0f);
	
	for(float index=-1.0;index<=1.0;index = index+0.05){
		drawLine(1.0f,-1.0f,index,1.0f,index,0.0f,0.0f,1.0f);
	}
	
	for(float index=-1.0;index<=1.0;index = index+0.05){
	drawLine(1.0f,index,-1.0f,index,1.0f,0.0f,0.0f,1.0f);
	}
}

void drawConcentricTriangles(float red,float green, float blue){
	for(float i=0.1;i<=1.1;i=i+0.1){
	glBegin(GL_LINE_LOOP);
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
		
		glVertex3f(0.0f,i,0.0f);
		glVertex3f(-i,-i,0.0f);
		glVertex3f(i,-i,0.0f);
	glEnd();
	}
}

