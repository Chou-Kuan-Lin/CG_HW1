#ifndef _CG_HW1_DBLPEN_H_
#define _CG_HW1_DBLPEN_H_

#include "Vector.hpp"

/**
 * This class is for representing a single double pendulum system abstractly.
 * 
*/

#define TEXTURE_NUM 2

class DoublePendulum
{
public:

	DoublePendulum(Vector3f rootPosition, float topRodLength, float bottomRodLength, float topBobWeight, float bottomBobWeight)
	{
		this->rootPosition = rootPosition;
		this->topRodLength = topRodLength;
		this->bottomRodLength = bottomRodLength;
		this->topBobWeight = topBobWeight;
		this->bottomBobWeight = bottomBobWeight;
	}

	void TextureInfo(GLuint texture[], Image* loadTexture_sun)
	{
		for (int i = 0;i < TEXTURE_NUM;i++)
			this->texture[i] = texture[i];
		this->loadTexture_sun = loadTexture_sun;
	}

	void render()
	{
		float LineWidth = 3;
		float Scale = powf(topBobWeight, 0.33) * 0.4;	//constant can any

		glLineWidth(LineWidth);

		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHTING);
		//上段
		glBegin(GL_LINES);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(rootPosition.x, rootPosition.y, rootPosition.z);
			glVertex3f(GetTopBobPosition().x, GetTopBobPosition().y, GetTopBobPosition().z);
		glEnd();
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);

		//方塊
		glPushMatrix();
		glTranslatef(GetTopBobPosition().x, GetTopBobPosition().y, GetTopBobPosition().z);
		//glRotatef(0, 1, 0, 0);	//x(上下)
		//glRotatef(0, 0, 1, 0);	//y(側)
		//弧度=角度*Pi/180
		glRotatef(topAngle/M_PI*180, 0, 0, 1);	//z(左右)
		glColor3f(0, 1, 0);
		glutSolidCube(Scale);
		glPopMatrix();

		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHTING);
		//下段
		glBegin(GL_LINES);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(GetTopBobPosition().x, GetTopBobPosition().y, GetTopBobPosition().z);
			glVertex3f(GetBottomBobPosition().x, GetBottomBobPosition().y, GetBottomBobPosition().z);
		glEnd();
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);

		//圓球
		GLUquadricObj* quadratic = gluNewQuadric();
		if (rootPosition.x == 0)
			glEnable(GL_TEXTURE_2D);
		
		glPushMatrix();
		glTranslatef(GetBottomBobPosition().x, GetBottomBobPosition().y, GetBottomBobPosition().z);
		glRotatef(bottomAngle / M_PI * 180, 0, 0, 1);
		if (rootPosition.x == 0)
		{
			//創造圓球紋理資訊
			glFrontFace(GL_CCW);
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			gluQuadricTexture(quadratic, GLU_TRUE);
			gluQuadricDrawStyle(quadratic, GLU_FILL);
			gluQuadricNormals(quadratic, GLU_SMOOTH);
			gluQuadricOrientation(quadratic, GLU_OUTSIDE);

			// generate OpenGL texture
			glEnable(GL_TEXTURE_2D);
			glGenTextures(1, texture);
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, loadTexture_sun->sizeX, loadTexture_sun->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loadTexture_sun->data);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	//不受最後的染色
		}
		else
		{
			glColor3f(1, 1, 1);	//小白球
		}

		gluSphere(quadratic, Scale, 10, 10);
		glPopMatrix();
		if (rootPosition.x == 0)
			glDisable(GL_TEXTURE_2D);

		//light
		GLfloat pos[] = { GetBottomBobPosition().x, GetBottomBobPosition().y, GetBottomBobPosition().z ,1 };
		glLightfv(GL_LIGHT1, GL_POSITION, pos);

		glFinish();
	}

	void updatePhysics(float deltaTime)
	{
		const float g = 9.8;

		const float m1 = topBobWeight;
		const float m2 = bottomBobWeight;
		const float a1 = topAngle;
		const float a2 = bottomAngle;
		const float r1 = topRodLength;
		const float r2 = bottomRodLength;
		const float a1_v = topAngleVelocity;
		const float a2_v = bottomAngleVelocity;

		// calculate topAngleAcceleration
		float num1 = -g * (2 * m1 + m2) * sin(a1);
		float num2 = -m2 * g * sin(a1 - 2 * a2);
		float num3 = -2 * sin(a1 - a2)*m2;
		float num4 = pow(a2_v, 2) + pow(a1_v, 2)*r1*cos(a1 - a2);
		float den = r1 * (2 * m1 + m2 - m2 * cos(2 * a1 - 2 * a2));

		topAngleAcceleration= (num1 + num2 + num3 * num4) / den;

		// calculate bottomAngleAcceleration
		num1 = 2 * sin(a1 - a2);
		num2 = pow(a1_v, 2)*r1*(m1 + m2);
		num3 = g * (m1 + m2) * cos(a1);
		num4 = pow(a2_v, 2)*r2*m2*cos(a1 - a2);
		den = r2 * 2 * (m1 + m2 - m2 * cos(2 * a1 - 2 * a2));

		bottomAngleAcceleration = num1 * (num2 + num3 + num4) / den;

		// apply and record
		topAngleVelocity += topAngleAcceleration * deltaTime;
		bottomAngleVelocity += bottomAngleAcceleration * deltaTime;

		topAngle += topAngleVelocity * deltaTime;
		bottomAngle += bottomAngleVelocity * deltaTime;
	}

public:
	float topRodLength;
	float topBobWeight;
	float bottomRodLength;
	float bottomBobWeight;

	// for rendering settings
	bool isLightSource = false;
	bool isUsingTexture = false;
	GLuint textureID = 0;

	Vector3f GetTopBobPosition()
	{
		//上節點位置
		return { rootPosition.x + topRodLength * sin(topAngle), rootPosition.y + topRodLength * cos(topAngle)*-1, rootPosition.z };
	}

	Vector3f GetBottomBobPosition()
	{
		//下節點位置
		return { GetTopBobPosition().x + bottomRodLength * sin(bottomAngle),GetTopBobPosition().y + bottomRodLength * cos(bottomAngle)*-1,GetTopBobPosition().z };
	}

private:
	Vector3f rootPosition;

	// for updating motion
	float topAngle = M_PI / 4; // in this template, this is represented in radian form
	float bottomAngle = 0; // in this template, this is represented in radian form

	float topAngleVelocity = 0;
	float bottomAngleVelocity = 0;

	float topAngleAcceleration = 0;
	float bottomAngleAcceleration = 0;

	//Texture Information
	Image* loadTexture_sun = new Image();
	GLuint texture[TEXTURE_NUM];
};

#endif