#include "MATRIX.h"

#include <glut.h>
#include <GL.H>
#include <GLU.H>

void Lighting(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);


	float light_pos[] = { 0.0, 400.0, 0.0, 0.0 };
	float light_dir[] = { 0, -1, 0 };
	float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	float light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	float light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	float matShininess = 200;
	float noMat[] = { 0, 0, 0, 1 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, noMat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, matShininess);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_dir);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

void Initialize(int argc, char** argv)
{
	glutInit(&argc, argv);

	// 윈도우 생성
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(1024, 576);
	glutCreateWindow("송도원조 project");
	
	// depth test
	glEnable(GL_DEPTH_TEST);

	// 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_CULL_FACE);

	Lighting();
}