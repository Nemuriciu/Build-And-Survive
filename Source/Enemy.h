#pragma once
using namespace glm;
using namespace std;

class Enemy : public GameObject
{
public:
	Enemy(vec3 pos)
	{
		lives = 10;
		position = pos;
		speed = 0.055f;
		zone = 0;
		sphereRadius = 1;
	};
	~Enemy(){};

	void drawObject()
	{
		if (position.x <= -10.0f && position.z == 10.0f) zone = 1;
		else if (position.x >= 10.0f && position.z <= -10.0f) zone = 2;
		else if (position.x <= -10.0f && position.z <= -10.0f) zone = 3;

		if (!isDying)
		{
			if (position.y >= 0.2) up = true;
			if (position.y <= 0) up = false;
			if (!up) position.y += 0.0015f;
			else position.y -= 0.0015f;
		}

		if (zone == 0)
		{
			enemyMatrix = mat4(1);
			enemyMatrix = translate(enemyMatrix, position);
			enemyMatrix = rotate(enemyMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
			enemyMatrix = rotate(enemyMatrix, angle, glm::vec3(0, 0, 1));
			enemyMatrix = rotate(enemyMatrix, dyingAngle, glm::vec3(1, 0, 0));
			enemyMatrix = scale(enemyMatrix, scaleSize);

			if(!isDying)
				position.x -= speed;
			else
			{
				if (dyingAngle < RADIANS(90))
				{
					dyingAngle += 0.1f;
				}
				else
				{
					if (scaleSize.x >= 0.000001f)
						scaleSize -= 0.0002f;
					else
						isDead = true;
				}
			}
		}
		else if (zone == 1)
		{
			if (angle > RADIANS(-225))
			{
				angle -= 0.2f;
				enemyMatrix = mat4(1);
				enemyMatrix = translate(enemyMatrix, position);
				enemyMatrix = rotate(enemyMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
				enemyMatrix = rotate(enemyMatrix, angle, glm::vec3(0, 0, 1));
				enemyMatrix = scale(enemyMatrix, scaleSize);
			}
			else
			{
				enemyMatrix = mat4(1);
				enemyMatrix = translate(enemyMatrix, position);
				enemyMatrix = rotate(enemyMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
				enemyMatrix = rotate(enemyMatrix, angle, glm::vec3(0, 0, 1));
				enemyMatrix = rotate(enemyMatrix, dyingAngle, glm::vec3(1, 0, 0));
				enemyMatrix = scale(enemyMatrix, scaleSize);

				if (!isDying)
				{
					position.x += speed;
					position.z -= speed;
				}
				else
				{
					if (dyingAngle < RADIANS(90))
						dyingAngle += 0.1f;
					else
					{
						if (scaleSize.x >= 0.000001f)
							scaleSize -= 0.0002f;
						else
							isDead = true;
					}
				}
				
			}
		}
		else if (zone == 2)
		{
			if (angle < RADIANS(-90))
			{
				angle += 0.2f;
				enemyMatrix = mat4(1);
				enemyMatrix = translate(enemyMatrix, position);
				enemyMatrix = rotate(enemyMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
				enemyMatrix = rotate(enemyMatrix, angle, glm::vec3(0, 0, 1));
				enemyMatrix = scale(enemyMatrix, scaleSize);
			}
			else
			{
				enemyMatrix = mat4(1);
				enemyMatrix = translate(enemyMatrix, position);
				enemyMatrix = rotate(enemyMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
				enemyMatrix = rotate(enemyMatrix, angle, glm::vec3(0, 0, 1));
				enemyMatrix = rotate(enemyMatrix, dyingAngle, glm::vec3(1, 0, 0));
				enemyMatrix = scale(enemyMatrix, scaleSize);
				
				if (!isDying)
					position.x -= speed;
				else
				{
					if (dyingAngle < RADIANS(90))
						dyingAngle += 0.1f;
					else
					{
						if (scaleSize.x >= 0.000001f)
							scaleSize -= 0.0002f;
						else
							isDead = true;
					}
				}
				
			}
		}
		else if (zone == 3) finished = true;
	}

public:
	mat4 enemyMatrix;
	vec3 scaleSize = vec3(0.015f);
	float lives;
	float speed;
	float angle = RADIANS(-90);
	float dyingAngle = RADIANS(0);
	int zone;
	bool up = false;
	bool isDead = false;
	bool isDying = false;
	bool finished = false;
};