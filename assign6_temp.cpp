/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 11/04/2013 - Assignment 6                  */

//#include <GL/glut.h>
#include "glut.h"
#include <GL/glu.h>
#include <GL/gl.h>
#include "glprocs.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <string.h>
#include <windows.h>

using namespace std;

#define PI 3.14159265359
#define RED 0
#define GREEN 1
#define BLUE 2
#define WHITE 3

//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,p;
int illimunationMode = 1;
int shadingMode = 1;
int lightSource = 0;

// Shader model declarations
void materialPropertesFileRead(char *fn);
float P_RGB_ambient[3];
float P_RGB_diffuse[3];
float P_RGB_specular[3];
float P_exp_specular;
float C_RGB_ambient[3];
float C_d;
float C_RGB_Rd[3];
float C_s;
float C_RGB_Fs[3];
float C_m[2];
float C_w[2];

//Projection, camera contral related declerations
int WindowWidth,WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;
float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;

// Color related declarations
int currentColor = RED;
float R = 1.0;
float G = 0.0;
float B = 0.0;

void DisplayFunc(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glEnable(GL_LIGHTING);

	GLfloat light0Color[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light1Color[] = {R, G, B, 1.0};

	GLfloat ambient[] = {0.02 , 0.02 , 0.02, 1.0};

	GLfloat light0Position [] = {CameraRadius*cos(CameraTheta)*sin(CameraPhi),
						CameraRadius*sin(CameraTheta)*sin(CameraPhi),
						CameraRadius*cos(CameraPhi),
						1.0};
	GLfloat light1Position [] = {7.0, 7.0, 7.0, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0Color);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Color);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1Color);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);

	if (lightSource == 0) {
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	} else {
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
	}
        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  0,0,0,
			  0,0,1);
	glEnable(GL_DEPTH_TEST);
	glutSolidTeapot(1);
	glutSwapBuffers();
}

void ReshapeFunc(int x,int y) {
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}

void MouseFunc(int button,int state,int x,int y) {
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y) {
	if(MouseLeft) {
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
	}
	if(MouseRight) {
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}

void setShaders() {
	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("shader.vert");
	fs = shaderFileRead("shader.frag");

	//set the shader's source code by using the strings read from the shader files.
	const char * vv = vs;
	const char * ff = fs;
	glShaderSourceARB(vertex_shader, 1, &vv,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff,NULL);
	free(vs);free(fs);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);

	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	//attach the shader objects to the program object
	glAttachObjectARB(p,vertex_shader);
	glAttachObjectARB(p,fragment_shader);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteObjectARB(vertex_shader);
	glDeleteObjectARB(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgramARB(p);

	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);

	GLint loc1, loc2, loc3, loc4, loc5, loc6, loc7, loc8, loc9, loc10, loc11, loc12, loc13, loc14;
	loc1 = glGetUniformLocationARB(p, "flag");
	glUniform1iARB(loc1, 2 * shadingMode + illimunationMode);
	loc2 = glGetUniformLocationARB(p, "pAmbientMat");
	glUniform3fvARB(loc2, 1, P_RGB_ambient);
	loc3 = glGetUniformLocationARB(p, "specularMat");
	glUniform3fvARB(loc3, 1, P_RGB_specular);
	loc4 = glGetUniformLocationARB(p, "diffuseMat");
	glUniform3fvARB(loc4, 1, P_RGB_diffuse);
	loc5 = glGetUniformLocationARB(p, "specularPower");
	glUniform1fARB(loc5, P_exp_specular);
	loc6 = glGetUniformLocationARB(p, "Rd");
	glUniform3fvARB(loc6, 1, C_RGB_Rd);
	loc7 = glGetUniformLocationARB(p, "F0");
	glUniform3fvARB(loc7, 1, C_RGB_Fs);
	loc8 = glGetUniformLocationARB(p, "d");
	glUniform1fARB(loc8, C_d);
	loc9 = glGetUniformLocationARB(p, "s");
	glUniform1fARB(loc9, C_s);
	loc10 = glGetUniformLocationARB(p, "m1");
	glUniform1fARB(loc10, C_m[0]);
	loc11 = glGetUniformLocationARB(p, "w1");
	glUniform1fARB(loc11, C_w[0]);
	loc12 = glGetUniformLocationARB(p, "m2");
	glUniform1fARB(loc12, C_m[1]);
	loc13 = glGetUniformLocationARB(p, "w2");
	glUniform1fARB(loc13, C_w[1]);
	loc14 = glGetUniformLocationARB(p, "cAmbientMat");
	glUniform3fvARB(loc14, 1, C_RGB_ambient);
}

void KeyboardFunc(unsigned char key, int x, int y) {
	GLint loc;

    switch(key) {
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'w':
	case 'W':
		if (illimunationMode == 0) {
			illimunationMode = 1;

		} else {
			illimunationMode = 0;
		}
		loc = glGetUniformLocationARB(p, "flag");
		glUniform1iARB(loc, 2 * shadingMode + illimunationMode);
		glutPostRedisplay();
		break;
	case 'e':
	case 'E':
		if (shadingMode == 0) {
			shadingMode =1;
		} else {
			shadingMode =0;
		}
		loc = glGetUniformLocationARB(p, "flag");
		glUniform1iARB(loc, 2 * shadingMode + illimunationMode);
		glutPostRedisplay();
		break;
	case 'd':
	case 'D':
		if (lightSource == 0) {
			lightSource =1;
		} else {
			lightSource =0;
		}
		break;
	case 'f':
	case 'F':
		if (lightSource == 1) {
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.
			if (currentColor == RED ) {
				R = 0.0;
				G = 1.0;
				B = 0.0;
				currentColor = GREEN;
			} else if (currentColor == GREEN) {
				R = 0.0;
				G = 0.0;
				B = 1.0;
				currentColor = BLUE;
			} else if (currentColor == BLUE) {
				R = 1.0;
				G = 1.0;
				B = 1.0;
				currentColor = WHITE;
			} else if (currentColor == WHITE) {
				R = 1.0;
				G = 0.0;
				B = 0.0;
				currentColor = RED;
			}
		}
		break;

    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char **argv)  {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Assignment 6");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

	const GLubyte *temp;
	temp=glGetString(GL_VERSION);
	printf("%s\n",temp);
	temp=glGetString(GL_VENDOR);
	printf("%s\n",temp);
	temp=glGetString(GL_EXTENSIONS);
	printf("%s\n",temp);
	
	materialPropertesFileRead("material.dat");

	setShaders();
	glutMainLoop();
	return 0;
}

//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {
	FILE *fp = fopen(fn,"r");
	if(!fp) {
		cout<< "Failed to load " << fn << endl;
		return " ";
	} else {
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);
		if (count > 0) {
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}

void materialPropertesFileRead(char *fn) {
	ifstream fp(fn, ios::in);
	if(!fp || !fp.is_open()) {
		cout<< "Failed to load " << fn << endl;
		return;
	}


	char type;
	while (!fp.eof()) {
		fp >> type;
		if (type == 'P') {
			fp >> P_RGB_ambient[0] >> P_RGB_ambient[1] >> P_RGB_ambient[2] >>
				  P_RGB_diffuse[0] >> P_RGB_diffuse[1] >> P_RGB_diffuse[2] >>
				  P_RGB_specular[0] >> P_RGB_specular[1] >> P_RGB_specular[2] >>
				  P_exp_specular;
		} else if (type == 'C') {
			fp >> C_RGB_ambient[0] >> C_RGB_ambient[1] >> C_RGB_ambient[2] >>
				C_d >> C_RGB_Rd[0] >> C_RGB_Rd[1] >> C_RGB_Rd[2] >>
				C_s >> C_RGB_Fs[0] >> C_RGB_Fs[1] >> C_RGB_Fs[2] >>
				C_m[0] >> C_w[0] >> C_m[1] >> C_w[1];
		}
	}

	fp.close();
}