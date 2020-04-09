#pragma warning(disable: 4996)
#include "VECTOR.h"
#include "Face.h"
#include "Mesh.h"
#include "Init.h"
#include <time.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define MAX_BALL_NUM 100
#define COR 0.995 // �ݹ� ���
#define BOX 500 //��� ũ��

using namespace std;

GLuint skybox[6];
static GLfloat mat_ambdif[] = { 0.8, 0.8, 0.8, 1.0 };
static GLfloat mat_spc[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shn[] = { 10.0 };

float back_size = 100.0f; // ������ü ũ��
float Gravity = 9.8 / 30.0;	// �߷�
float camX = 50.0f;		// ī�޶� X��ǥ
float camY = 150.0f;		// ī�޶� Y��ǥ
float camZ = 400.0f;		// ī�޶� Z��ǥ
float lookAtX = 50.0f;			// ī�޶� �ٶ󺸴� X��ǥ
float lookAtY = 30.0f;			// ī�޶� �ٶ󺸴� Y��ǥ
float lookAtZ = -100.0f;			// ī�޶� �ٶ󺸴� Z��ǥ
int prevMousePosX, prevMousePosY; // ���콺 Ŭ�� �� ��ǥ�� ����
int diffMousePosX, diffMousePosY; // ���콺 �巡�׽� ��ǥ�� ��ȭ ����
int num_ball = MAX_BALL_NUM; // ���� ����
int now_num; // ���� ������ ���� ����

time_t startTime, endTime; //�ð��� ���� ���� ����
float last_time = 0;

bool isCaseOpened = false;
bool blackBoxMode = false;

struct object {
	float posx, posy, posz; //��ġ ��ǥ ��
	float velx, vely, velz; // �ӵ� ����
	float cloR, cloG, cloB; //���� ��

	GLUquadricObj *obj;
}ball[MAX_BALL_NUM];

GLuint loadTexture(char* texfile) {
	GLuint texture = NULL;
	glGenTextures(1, &texture);// texture ������ �ϳ� �����

							   //�̹��� �ε��ϰ�
	FILE *fp = fopen(texfile, "rb");
	if (!fp) {
		printf("ERROR : No %s.\n fail to bind %d\n", texfile, texture);
		return false;
	}

	int width, height, channel;
	unsigned char *image = stbi_load_from_file(fp, &width, &height, &channel, 4);
	fclose(fp);

	printf("%s %d %d\n", texfile, width, height);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
	
	return texture;
}


void Makeball()
{
	srand((unsigned int)time(NULL));

	int i, a, b, c;
	for (i = 0; i < num_ball; i++) {
		if (i % 4 == 0) {
			ball[i].posx = 6;
			ball[i].posy = 94;
			ball[i].posz = 6;
			ball[i].velx = 10.0f + rand() % 3;
			ball[i].velz = 10.0f + rand() % 3;
		}
		else if (i % 4 == 1) {
			ball[i].posx = 94;
			ball[i].posy = 94;
			ball[i].posz = 6;
			ball[i].velx = (-1)* (10.0f + rand() % 3);
			ball[i].velz = 10.0f + rand() % 3;
		}
		else if (i % 4 == 2) {
			ball[i].posx = 94;
			ball[i].posy = 94;
			ball[i].posz = 94;
			ball[i].velx = (-1)* (10.0f + rand() % 3);
			ball[i].velz = (-1)* (10.0f + rand() % 3);
		}
		else if (i % 4 == 3) {
			ball[i].posx = 6;
			ball[i].posy = 94;
			ball[i].posz = 94;
			ball[i].velx = 10.0f + rand() % 3;
			ball[i].velz = (-1)* (10.0f + rand() % 3);
		}
		ball[i].vely = -10.0f;

		a = rand() % 256;
		b = rand() % 256;
		c = rand() % 256;

		ball[i].cloR = (double)a / 256;
		ball[i].cloG = (double)b / 256;
		ball[i].cloB = (double)c / 256;
		ball[i].obj = gluNewQuadric();
	}
}

void Mouse(int mouse_event, int state, int x, int y)
{
	if (mouse_event == GLUT_DOWN) {
		prevMousePosX = x;
		prevMousePosY = y;
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	diffMousePosX = x - prevMousePosX;
	diffMousePosY = prevMousePosY - y;
	lookAtX += diffMousePosX / 2.0f;
	lookAtY += diffMousePosY / 2.0f;
	prevMousePosX = x;
	prevMousePosY = y;

	glutPostRedisplay();
}

void RenderBox() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambdif);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_ambdif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spc);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shn);

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, skybox[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1 - 1.0);
	glVertex3f(BOX, BOX, -BOX);
	glTexCoord2f(0.0, 1 - 0.0);
	glVertex3f(BOX, BOX, BOX);
	glTexCoord2f(1.0, 1 - 0.0);
	glVertex3f(-BOX, BOX, BOX);
	glTexCoord2f(1.0, 1 - 1.0);
	glVertex3f(-BOX, BOX, -BOX);
	glEnd();


	glBindTexture(GL_TEXTURE_2D, skybox[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1 - 1.0);
	glVertex3f(-BOX, -0.5, -BOX);
	glTexCoord2f(0.0, 1 - 1.0);
	glVertex3f(-BOX, -0.5, BOX);
	glTexCoord2f(0.0, 1 - 0.0);
	glVertex3f(BOX, -0.5, BOX);
	glTexCoord2f(1.0, 1 - 0.0);
	glVertex3f(BOX, -0.5, -BOX);
	glEnd();

	
	glBindTexture(GL_TEXTURE_2D, skybox[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1 - 1.0);
	glVertex3f(-BOX, BOX, -BOX);
	glTexCoord2f(0.0, 1 - 1.0);
	glVertex3f(-BOX, BOX, BOX);
	glTexCoord2f(0.0, 1 - 0.0);
	glVertex3f(-BOX, -0.5, BOX);
	glTexCoord2f(1.0, 1 - 0.0);
	glVertex3f(-BOX, -0.5, -BOX);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1 - 1.0);
	glVertex3f(BOX, BOX, -BOX);
	glTexCoord2f(0.0, 1 - 0.0);
	glVertex3f(BOX, -0.5, -BOX);
	glTexCoord2f(1.0, 1 - 0.0);
	glVertex3f(BOX, -0.5, BOX);
	glTexCoord2f(1.0, 1 - 1.0);
	glVertex3f(BOX, BOX, BOX);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1 - 0.0);
	glVertex3f(BOX, -0.5, -BOX);
	glTexCoord2f(1.0, 1 - 1.0);
	glVertex3f(BOX, BOX, -BOX);
	glTexCoord2f(0.0, 1 - 1.0);
	glVertex3f(-BOX, BOX, -BOX);
	glTexCoord2f(0.0, 1 - 0.0);
	glVertex3f(-BOX, -0.5, -BOX);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1 - 1.0);
	glVertex3f(BOX, BOX, BOX);
	glTexCoord2f(0.0, 1 - 0.0);
	glVertex3f(BOX, -0, BOX);
	glTexCoord2f(1.0, 1 - 0.0);
	glVertex3f(-BOX, -0, BOX);
	glTexCoord2f(1.0, 1 - 1.0);
	glVertex3f(-BOX, BOX, BOX);
	glEnd();
}

void RenderPlane()
{
	glColor3f(0.3f, 0.3f, 0.5f);
	if (blackBoxMode) {		// ���ڽ� ����� ��
		// Left
		glBegin(GL_QUADS);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, back_size, 0.0f);
		glVertex3f(0.0f, back_size, back_size);
		glVertex3f(0.0f, 0.0f, back_size);
		glEnd();

		// Right
		glBegin(GL_QUADS);
		if (camX <= 20.0f) {
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glVertex3f(back_size, 0.0f, 0.0f);
			glVertex3f(back_size, 0.0f, back_size);
			glVertex3f(back_size, back_size, back_size);
			glVertex3f(back_size, back_size, 0.0f);
		}
		else {
			glNormal3f(1.0f, 0.0f, 0.0f);
			glVertex3f(back_size, 0.0f, 0.0f);
			glVertex3f(back_size, 0.0f, back_size);
			glVertex3f(back_size, back_size, back_size);
			glVertex3f(back_size, back_size, 0.0f);
		}
		glEnd();

		// Front
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(back_size, 0.0f, back_size);
		glVertex3f(back_size, back_size, back_size);
		glVertex3f(0.0f, back_size, back_size);
		glVertex3f(0.0f, 0.0f, back_size);
		glEnd();

		// Back
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(back_size, 0.0f, 0.0f);
		glVertex3f(back_size, back_size, 0.0f);
		glVertex3f(0.0f, back_size, 0.0f);
		glEnd();

		// Top
		glBegin(GL_QUADS);
		if (!isCaseOpened) {
			if (camX <= 20.0f) {
				glNormal3f(0.0f, -1.0f, 0.0f);
				glVertex3f(0.0f, back_size, 0.0f);
				glVertex3f(back_size, back_size, 0.0f);
				glVertex3f(back_size, back_size, back_size);
				glVertex3f(0.0f, back_size, back_size);				
			}
			else {
				glNormal3f(0.0f, 1.0f, 0.0f);
				glVertex3f(0.0f, back_size, 0.0f);
				glVertex3f(0.0f, back_size, back_size);
				glVertex3f(back_size, back_size, back_size);
				glVertex3f(back_size, back_size, 0.0f);
			}
		}
		else {
			; // �Ѳ������� ������ �׸��� ����
		}
		glEnd();

		// Bottom
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, back_size);
		glVertex3f(back_size, 0.0f, back_size);
		glVertex3f(back_size, 0.0f, 0.0f);
		glEnd();
	}
	else {		// ���ڽ� ��尡 �ƴ� ��
		// Left
		glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, back_size);
		glVertex3f(0.0f, back_size, back_size);
		glVertex3f(0.0f, back_size, 0.0f);
		glEnd();

		// Back
		glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, back_size, 0.0f);
		glVertex3f(back_size, back_size, 0.0f);
		glVertex3f(back_size, 0.0f, 0.0f);
		glEnd();

		// Right
		glBegin(GL_LINE_LOOP);
		glVertex3f(back_size, 0.0f, 0.0f);
		glVertex3f(back_size, 0.0f, back_size);
		glVertex3f(back_size, back_size, back_size);
		glVertex3f(back_size, back_size, 0.0f);
		glEnd();

		// Front
		glBegin(GL_LINE_LOOP);
		glVertex3f(back_size, 0.0f, back_size);
		glVertex3f(0.0f, 0.0f, back_size);
		glVertex3f(0.0f, back_size, back_size);
		glVertex3f(back_size, back_size, back_size);
		glEnd();
	}
}

void RenderBall() {
	int i;
	for (i = 0; i < now_num; i++) {
		glLoadIdentity();
		gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, 0.0f, 1.0f, 0.0f);

		glColor3f(ball[i].cloR, ball[i].cloG, ball[i].cloB);
		glTranslatef(ball[i].posx, ball[i].posy, ball[i].posz);
		gluSphere(ball[i].obj, 5.0, 20, 20);
	}
}

void Rendering(void)
{
	glEnable(GL_TEXTURE_2D);

	// ȭ�� ���� Ŭ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ȭ���� ����� �ٶ󺸱� ���� ī�޶� ȸ�� �� �̵�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, 0.0f, 1.0f, 0.0f);

	RenderBox();
	RenderPlane();
	RenderBall();

	// back ���ۿ� �������� �� swap
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	// ����Ʈ ����
	glViewport(0, 0, w, h);

	// ���� ���� ���
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 2000);

	// �𵨺� ��Ʈ���� �ʱ�ȭ
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'o':
	case 'O':
		isCaseOpened = true;
		for (int i = 0; i < now_num; i++) ball[i].vely = -200.0;
		break;
	case 'w':
		camX -= 1.0f;
		camY -= 3.0f;
		camZ -= 8.0f;
		break;
	case 's':
		camX += 1.0f;
		camY += 3.0f;
		camZ += 8.0f;
		break;
	case 'b':
		blackBoxMode = !blackBoxMode;
		break;
	}

	glutPostRedisplay();
}

void idle() {
	endTime = clock();
	float gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC);  // �ð� ���� ���
	
	if (last_time <= gap && now_num < num_ball) { // ���ο� �� ����
		now_num++;
		last_time += 0.3;
	}

	int i;
	for (i = 0; i < now_num; i++) {  // ���� ��ġ ����
		ball[i].posx += ball[i].velx*0.01;
		ball[i].posy += ball[i].vely*0.01;
		ball[i].posz += ball[i].velz*0.01;

		if (!isCaseOpened || ((ball[i].posx >= 0.0f && ball[i].posx <= back_size) && (ball[i].posy >= 0.0f && ball[i].posy <= back_size - 5.0f) && (ball[i].posz >= 0.0f && ball[i].posz <= back_size))) {
			if (ball[i].posx < 5.0f || ball[i].posx > back_size - 5.0f) {
				ball[i].velx = 0 - ball[i].velx * COR;
				ball[i].vely *= COR;
				ball[i].velz *= COR;
			}
			if ((ball[i].posx + ball[i].velx * 0.01) < 5.0f) ball[i].posx = 5.0f;
			if ((ball[i].posx + ball[i].velx * 0.01 > back_size - 5.0f)) ball[i].posx = back_size - 5.0f;
			if (ball[i].posy < 5.0f || ball[i].posy > back_size - 5.0f) {
				ball[i].velx *= COR;
				ball[i].vely = 0 - ball[i].vely * COR;
				ball[i].velz *= COR;
			}
			if ((ball[i].posy + ball[i].vely * 0.01) < 5.0f) ball[i].posy = 5.0f;
			if ((ball[i].posy + ball[i].vely * 0.01 > back_size - 5.0f)) ball[i].posy = back_size - 5.0f;
			if (ball[i].posz < 5.0f || ball[i].posz > back_size - 5.0f) {
				ball[i].velx *= COR;
				ball[i].vely *= COR;
				ball[i].velz = 0 - ball[i].velz * COR;
			}
			if ((ball[i].posz + ball[i].velz * 0.01) < 5.0f) ball[i].posz = 5.0f;
			if ((ball[i].posz + ball[i].velz * 0.01 > back_size - 5.0f)) ball[i].posz = back_size - 5.0f;
			ball[i].vely -= Gravity;
		}
		else {
			if (ball[i].posy < 5.0f) {
				ball[i].velx *= COR;
				ball[i].vely = 0 - ball[i].vely * COR;
				ball[i].velz *= COR;
			}
			if ((ball[i].posy + ball[i].vely * 0.01) < 5.0f) ball[i].posy = 5.0f;
			ball[i].vely -= Gravity;
		}

	}

	for (int i = 0; i < now_num; i++) {		// ���� ���� �浹
		for (int j = i + 1; j < now_num; j++) {
			if (i != j) {
				float dx = ball[i].posx - ball[j].posx;
				float dy = ball[i].posy - ball[j].posy;
				float dz = ball[i].posz - ball[j].posz;

				float distance_square = dx * dx + dy * dy + dz * dz;

				if (distance_square < 100.0f) {
					VECTOR3D vel_now_i(ball[i].velx, ball[i].vely, ball[i].velz);
					VECTOR3D between(0 - dx, 0 - dy, 0 - dz);

					//float magTemp_i = vel_now_i.InnerProduct(between) / (vel_now_i.Magnitude() * between.Magnitude());
					float magTemp_i = vel_now_i.InnerProduct(between) / (between.Magnitude() * between.Magnitude());
					VECTOR3D center_i(0 - dx * magTemp_i, 0 - dy * magTemp_i, 0 - dz * magTemp_i);
					VECTOR3D normal_i = vel_now_i - center_i;

					VECTOR3D vel_now_j(ball[j].velx, ball[j].vely, ball[j].velz);
					between = VECTOR3D(dx, dy, dz);

					//float magTemp_j = vel_now_j.InnerProduct(between) / (vel_now_j.Magnitude() * between.Magnitude());
					float magTemp_j = vel_now_j.InnerProduct(between) / (between.Magnitude() * between.Magnitude());
					VECTOR3D center_j(dx * magTemp_j, dy * magTemp_j, dz * magTemp_j);
					VECTOR3D normal_j = vel_now_j - center_j;

					VECTOR3D temp = center_i;
					center_i = center_j * COR;
					center_j = temp * COR;

					vel_now_i = center_i + normal_i;
					vel_now_j = center_j + normal_j;

					ball[i].velx = vel_now_i.x;
					ball[i].vely = vel_now_i.y;
					ball[i].velz = vel_now_i.z;

					ball[j].velx = vel_now_j.x;
					ball[j].vely = vel_now_j.y;
					ball[j].velz = vel_now_j.z;

					VECTOR3D center_dir(dx, dy, dz);
					VECTOR3D newPos = center_dir * (10.0f / center_dir.Magnitude());
					ball[i].posx = ball[j].posx + newPos.x;
					ball[i].posy = ball[j].posy + newPos.y;
					ball[i].posz = ball[j].posz + newPos.z;
				}
			}
		}
	}
	glutPostRedisplay();
}

void EventHandlingAndLoop()
{
	startTime = clock();

	glutKeyboardFunc(Keyboard);  // Ű���� �Է� ó�� Callback �Լ� ���
	glutDisplayFunc(Rendering);  // ��ȯ�� ���� ���� Rendering Callback �Լ� ���
	glutReshapeFunc(Reshape);    // ������ â ũ�Ⱑ �ٲ������ ȣ��Ǵ� Callback �Լ� ���
	glutMouseFunc(Mouse);        // ���콺 Ŭ�� ó�� Callback �Լ� ���
	glutMotionFunc(Motion);      // ���콺 ������ ó�� Callback �Լ� ���
	glutIdleFunc(idle);

	glutMainLoop(); // ��ϵ� callback �Լ��� �ݺ��Ͽ� ȣ��
}

int main(int argc, char** argv)
{
	Initialize(argc, argv);			  // ������ ����, ���� ����

	skybox[0] = loadTexture((char*)".\\texture\\top.tga");
	skybox[1] = loadTexture((char*)".\\texture\\bottom.tga");
	skybox[2] = loadTexture((char*)".\\texture\\left.tga");
	skybox[3] = loadTexture((char*)".\\texture\\right.tga");
	skybox[4] = loadTexture((char*)".\\texture\\front.tga");
	skybox[5] = loadTexture((char*)".\\texture\\back.tga");

	Makeball(); //�� ���� ���� ����

	EventHandlingAndLoop();      // Event Handling �� Loop
	return 0;					// ���� ���� ������ ��� 0�� ������
}