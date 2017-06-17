#pragma once
using namespace glm;

class Bullet : public GameObject
{
public:
	Bullet(float size, vec3 pos)
	{
		bulletSize = size;
		position = pos;
		if(bulletSize == 2)	position.y += 0.1;
		else position.y += 0.5;

		sphereRadius = 0.1f;
	};
	~Bullet() {};

	void drawObject(bool fixed)
	{
		if (!fixed)
		{
			distTraveled += abs(speed *  cos(angle + RADIANS(180)));
			distTraveled += abs(speed * sin(angle + RADIANS(180)));

			position.z -= speed * cos(angle + RADIANS(180));
			position.x -= speed * sin(angle + RADIANS(180));
			bulletMatrix = mat4(1);
			bulletMatrix = translate(bulletMatrix, position);

			if (bulletSize == 3)
			{
				position.y += acceleration;
				acceleration -= 0.0005f;
				bulletMatrix = scale(bulletMatrix, vec3(0.25f));
			}
			else
				bulletMatrix = scale(bulletMatrix, vec3(0.1f));
		}
		else
		{
			float dirX = fixedPosition.x - position.x;
			float dirY = fixedPosition.y - position.y;
			float dirZ = fixedPosition.z - position.z;

			float hypo1 = sqrt(dirX * dirX + dirZ * dirZ);
			float hypo2 = sqrt(dirX * dirX + dirY * dirY);
			dirX /= hypo1;
			dirZ /= hypo1;
			dirY /= hypo2;

			position.x += dirX * speed;
			position.z += dirZ * speed;
			position.y += dirY * speed;

			fixedPosition.x += dirX * speed;
			fixedPosition.y += dirY * speed;
			fixedPosition.z += dirZ * speed;

			bulletMatrix = mat4(1);
			bulletMatrix = translate(bulletMatrix, position);
			bulletMatrix = scale(bulletMatrix, vec3(0.2f));
		}
	}

public:
	mat4 bulletMatrix;
	vec3 fixedPosition = vec3(0);
	float acceleration = 0.025f;
	float speed = 0.2f;
	float bulletSize = 0;
	float angle = 0;
	float distTraveled = 0;
	bool up = false;
};