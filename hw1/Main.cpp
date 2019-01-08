/*HW1-Double Pendulum Systems

<insert TA's words...>

*/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

/**
 * this needs to be at the top, since some headers might already includes the
 * cmath header. So to prevent including cmath without _USE_MATH_DEFINES, put it on top of everything else.
 */
#define _USE_MATH_DEFINES

#include "./GL/glut.h"
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "Image.hpp"
#include "DoublePendulum.hpp"

#include <string>

//number of textures desired, you may want to change it to get bonus point
#define TEXTURE_NUM 2

//大小設定
float objectSize = 10;
float windowLength = 800;
float windowWidth = 600;

//directories of image files
char* texture_name[TEXTURE_NUM] = {
	"./Resource/sun.bmp", // 0
	"./Resource/ceiling.bmp", // 1
};
//texture id array
GLuint texture[TEXTURE_NUM];
Image* loadTexture_ceiling = new Image();
Image* loadTexture_sun = new Image();

//passTime parameter for helping coordinate your animation, you may utilize it to help perform animation
#define deltaTime 10 // in ms (1e-3 second)
double passTime = 0.0;

//modification of these parameter is forbidden in this homework
float LightPos[] = { 0.0f, 0.0f, 1.0f, 1.0f };//Light position
float LightAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Ambient Light Values
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Specular Light Values

// The double pendulum systems
DoublePendulum* doublePendulumSystems[3];

//攝像機參數
GLdouble eyeX = 0;
GLdouble eyeY = 0;
GLdouble eyeZ = objectSize* -3;
GLdouble centerX = 0;
GLdouble centerY = 0;
GLdouble centerZ = 0;
GLdouble upX = 0;	//相機朝向
GLdouble upY = 1;
GLdouble upZ = 0;
//float CameraPosition[] = { eyeX, eyeY, eyeZ };

//貼紋理
void PasteTexture(int num, GLuint texture[], Image* loadTexture)
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, texture[num]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, loadTexture->sizeX, loadTexture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loadTexture->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void RenderPlanes()
{
	//紋理
	PasteTexture(1, texture, loadTexture_ceiling);

	//盒子五面
	//底面
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(objectSize, -objectSize, objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-objectSize, -objectSize, objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-objectSize, objectSize, objectSize);
	glEnd();

	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-objectSize, objectSize, objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(objectSize, objectSize, objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(objectSize, -objectSize, objectSize);
	glEnd();

	//上面
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(objectSize, objectSize, objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(objectSize, objectSize, -objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-objectSize, objectSize, -objectSize);
	glEnd();

	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(objectSize, objectSize, objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-objectSize, objectSize, -objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-objectSize, objectSize, objectSize);
	glEnd();

	//左面
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(objectSize, objectSize, objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(objectSize, -objectSize, -objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(objectSize, objectSize, -objectSize);
	glEnd();

	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(objectSize, -objectSize, -objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(objectSize, objectSize, objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(objectSize, -objectSize, objectSize);
	glEnd();

	//右面
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-objectSize, -objectSize, -objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-objectSize, -objectSize, objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-objectSize, objectSize, objectSize);
	glEnd();

	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-objectSize, -objectSize, -objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-objectSize, objectSize, objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-objectSize, objectSize, -objectSize);
	glEnd();

	//下面
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(objectSize, -objectSize, objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-objectSize, -objectSize, -objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(objectSize, -objectSize, -objectSize);
	glEnd();

	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(objectSize, -objectSize, objectSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-objectSize, -objectSize, objectSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-objectSize, -objectSize, -objectSize);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

//callback function for drawing a frame
void Display(void)
{
	//繪幀區
		
	// Alway Clear Color and Depth Bit Before Drawing!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// Load Identity Matrix
	glLoadIdentity();

	// Setup Camera Configurations
	gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);

	RenderPlanes();
	
	// Render the double pendulum systems
	for (int i = 0; i < 3; i++)
	{
		doublePendulumSystems[i]->render();	
	}

	// Swap the drawn buffer to the window
	glutSwapBuffers();
}

//callback funtion as a passTimer, no need to modify it
void Tick(int id)
{
	double d = deltaTime / 1000.0;
	passTime += d;

	// update your physics here
	for (int i = 0; i < 3; i++)
	{
		// here, we can multiply d with a constant to speed up simulation
		if(doublePendulumSystems[i])
			doublePendulumSystems[i]->updatePhysics(d * 3);

		if (i == 1)
			doublePendulumSystems[1]->TextureInfo(texture, loadTexture_sun);
	}

	glutPostRedisplay();
	glutTimerFunc(deltaTime, Tick, 0); // 100ms for passTime step size
}

//callback function when the window size is changed, no need to modify it
void WindowSize(int w, int h)
{
	glViewport(0, 0, w, h);							//changing the buffer size the same to the window size
	glMatrixMode(GL_PROJECTION);					//choose the projection matrix
	glLoadIdentity();
	gluPerspective(60.0, (double)w /(double)h, 1.0, 1000.0);//set the projection matrix as perspective mode
	glMatrixMode(GL_MODELVIEW);						//it is suggested that modelview matrix is chosen if nothing specific being performed
	glLoadIdentity();
}

//initialization for parameters of this program, you must perform something here
void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);//set what color should be used when we clean the color buffer
	glEnable(GL_LIGHT1);//Enable Light1
	glEnable(GL_LIGHTING);//Enable Lighting
	//glLightfv(GL_LIGHT1, GL_POSITION, LightPos);//Set Light1 Position, this setting function should be at another place
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);//燈光的環境RGBA強度
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);//燈光的漫反射RGBA強度
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);//燈光的鏡面RGBA強度。
	//since a light source is also an object in the 3D scene, we set its position in the display function after gluLookAt()
	//you should know that straight texture mapping(without modifying shader) may not have shading effect
	//you need to tweak certain parameter(when building texture) to obtain a lit and textured object
	glShadeModel(GL_SMOOTH);//shading model	//這是TA您原本寫在CODE中的

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);//you can choose which part of lighting model should be modified by texture mapping
	glEnable(GL_COLOR_MATERIAL);//enable this parameter to use glColor() as material of lighting model
	
	//load all the textures here
	ImageLoad(texture_name[1], loadTexture_ceiling);
	ImageLoad(texture_name[0], loadTexture_sun);
	
	doublePendulumSystems[0] = new DoublePendulum({ objectSize/2, objectSize, -objectSize/2 },3.0f, 8.0f, 1.0f, 1.0f);
	doublePendulumSystems[1] = new DoublePendulum({ 0.0f, objectSize, 0.0f }, 5.0f, 5.0f, 1.0f, 1.0f);
	doublePendulumSystems[2] = new DoublePendulum({ -objectSize/2, objectSize, objectSize/2 }, 8.0f, 3.0f, 1.0f, 1.0f);
}

void DealKeyboardInput(unsigned char key, int x, int y)
{	
	std::string option = "";
	
	// 鍵盤控制攝影機方向
	switch (key)
	{
		// X軸向
		case 'q':	
		{
			eyeX++;
			option = "X axis eye";
			break;
		}
		case 'w':
		{
			eyeX--;
			option = "X axis eye";
			break;
		}
		case 'e':
		{
			centerX++;
			option = "X axis center";
			break;
		}
		case 'r':
		{
			centerX--;
			option = "X axis center";
			break;
		}
		case 't':
		{
			upX += 0.1;
			option = "X axis up";
			break;
		}
		case 'y':
		{
			upX -= 0.1;
			option = "X axis up";
			break;
		}
		// Y軸向
		case 'a':	
		{
			eyeY++;
			option = "Y axis eye";
			break;
		}
		case 's':
		{
			eyeY--;
			option = "Y axis eye";
			break;
		}
		case 'd':
		{
			centerY++;
			option = "Y axis center";
			break;
		}
		case 'f':
		{
			centerY--;
			option = "Y axis center";
			break;
		}
		case 'g':
		{
			upY += 0.1;
			option = "Y axis up";
			break;
		}
		case 'h':
		{
			upY -= 0.1;
			option = "Y axis up";
			break;
		}
		// Z軸向
		case 'z':
		{
			eyeZ++;
			option = "Z axis eye";
			break;
		}
		case 'x':
		{
			eyeZ--;
			option = "Z axis eye";
			break;
		}
		case 'c':
		{
			centerZ++;
			option = "Z axis center";
			break;
		}
		case 'v':
		{
			centerZ--;
			option = "Z axis center";
			break;
		}
		case 'b':
		{
			upZ += 0.1;
			option = "Z axis up";
			break;
		}
		case 'n':
		{
			upZ -= 0.1;
			option = "Z axis up";
			break;
		}
		//m1
		case 'u':
		{
			doublePendulumSystems[0]->topRodLength += 0.1;
			option = "r1 top length";
			break;
		}
		case 'i':
		{
			doublePendulumSystems[0]->topRodLength -= 0.1;
			option = "r1 top length";
			break;
		}
		case 'o':
		{
			doublePendulumSystems[0]->bottomRodLength += 0.1;
			option = "r1 bottom length";
			break;
		}
		case 'p':
		{
			doublePendulumSystems[0]->bottomRodLength -= 0.1;
			option = "r1 bottom length";
			break;
		}
		//m2
		case 'j':
		{
			doublePendulumSystems[1]->topRodLength += 0.1;
			option = "r2 top length";
			break;
		}
		case 'k':
		{
			doublePendulumSystems[1]->topRodLength -= 0.1;
			option = "r2 top length";
			break;
		}
		case 'l':
		{
			doublePendulumSystems[1]->bottomRodLength += 0.1;
			option = "r2 bottom length";
			break;
		}
		case ';':
		{
			doublePendulumSystems[1]->bottomRodLength -= 0.1;
			option = "r2 bottom length";
			break;
		}
		//m3
		case 'm':
		{
			doublePendulumSystems[2]->topRodLength += 0.1;
			option = "r3 top length";
			break;
		}
		case ',':
		{
			doublePendulumSystems[2]->topRodLength -= 0.1;
			option = "r3 top length";
			break;
		}
		case '.':
		{
			doublePendulumSystems[2]->bottomRodLength += 0.1;
			option = "r3 bottom length";
			break;
		}
		case '/':
		{
			doublePendulumSystems[2]->bottomRodLength -= 0.1;
			option = "r3 bottom length";
			break;
		}
	}

	// Deal with Inputs from Keyboard
	std::cout << "You had clicked " << key << "and changed " << option << "." << std::endl;
}

int main(int argc, char *argv[])
{
	//操作說明
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "You can control camera by keyboard: " << std::endl;
	std::cout << "  X axis by q.w(eye), e.r(center), t.y(up)" << std::endl;
	std::cout << "  Y axis by a.s(eye), d.f(center), f.g(up)" << std::endl;
	std::cout << "  Z axis by z.x(eye), c.v(center), b.n(up)" << std::endl;
	std::cout << "  r1 length by u.i.o.p" << std::endl;
	std::cout << "  r2 length by j.k.l.;" << std::endl;
	std::cout << "  r3 length by m.,.../" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	glutInit(&argc, argv);//glut function for simplifying OpenGL initialization
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//demanding: double-framed buffer | RGB colorbuffer | depth buffer
	glutInitWindowPosition(100, 50);//set the initial window position
	glutInitWindowSize(windowLength, windowWidth);//set the initial window size
	//**************************
	glutCreateWindow("CG_HW1_0756616");//IMPORTANT!! Create the window and set its title, please replace 12345678 with your own student ID
	//**************************
	glutDisplayFunc(Display);//callback funtion for drawing a frame
	glutReshapeFunc(WindowSize);//callback function when the window size is changed
	glutTimerFunc(deltaTime, Tick, 0);//passTimer function
	glutKeyboardFunc(DealKeyboardInput);

	//you may want to write your own callback funtion for other events(not demanded nor forbidden)
	init();//self-defined initialization function for the elegance of your code

	glutMainLoop();
	return 0;
}