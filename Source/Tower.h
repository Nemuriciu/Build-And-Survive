#pragma once
using namespace glm;
using namespace std;

class Tower : public GameObject
{
public:
	Tower(vec3 pos)
	{
		position = pos;
		sphereRadius = 4;
	};
	~Tower() {};

	void drawObject()
	{
		towerMatrix = mat4(1);
		towerMatrix = translate(towerMatrix, position);
		towerMatrix = scale(towerMatrix, vec3(0.0025f));
		towerMatrix = rotate(towerMatrix, angle, vec3(0, 1, 0));
	}

public:
	Bullet *bullet;
	mat4 towerMatrix;
	float angle;
	double lastShot;
};