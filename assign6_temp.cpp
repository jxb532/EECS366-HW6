/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 11/04/2013 - Assignment 6                  */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string.h>
#include <windows.h>
//#include <GL/glut.h>
#include "glut.h"
#include <GL/glu.h>
#include <GL/gl.h>
#include "glprocs.h"

using namespace std;

#define PI 3.14159265359


//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,p;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;

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

double R = 1.0;
double G = 1.0;
double B = 1.0;

void DisplayFunc(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	 GLfloat color[ ] = {R, G, B, 1.0};
        GLfloat ambient[ ] = {0.0 , 0.0 , 0.0, 1.0};
        GLfloat light0Position [ ] = {7.0, 7.0, 7.0};
        //GLfloat light1PosType [ ] = {CameraRadius*cos(CameraTheta)*sin(CameraPhi),
        //                  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
        //                  CameraRadius*cos(CameraPhi)};
        //GLfloat light1Direct [] = {CameraRadius*cos(CameraTheta)*sin(CameraPhi),
        //                  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
        //                  CameraRadius*cos(CameraPhi)};
        glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
       // glLightfv(GL_LIGHT1, GL_POSITION, light1PosType);
        //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1Direct);
        //glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
        //glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.5);
        glLightfv(GL_LIGHT0, GL_COLOR, color);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

		glEnable(GL_LIGHT0);

        //if(Light)
        //{
        //        glEnable (GL_LIGHT3);
        //    glDisable(GL_LIGHT1);
        //}
        //if(!Light)
        //{
        //                glEnable(GL_LIGHT1);
        //            glDisable(GL_LIGHT3);
        //}

        
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
	vs = shaderFileRead("sampleshader.vert");
	fs = shaderFileRead("sampleshader.frag");

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
}


//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y) {
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
		break;
	case 'e':
	case 'E':
		if (shadingMode == 0) {
			shadingMode =1;
		} else {
			shadingMode =0;
		}
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