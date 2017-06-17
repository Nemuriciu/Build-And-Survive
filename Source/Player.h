#pragma once
using namespace glm;

class Player : public GameObject
{
public:
	Player(){};
	~Player(){};

	void drawObject(float angle)
	{
		playerMatrix = mat4(1);
		playerMatrix = translate(playerMatrix, position);
		playerMatrix = rotate(playerMatrix, angle, glm::vec3(0, 1, 0));
		playerMatrix = scale(playerMatrix, glm::vec3(0.004f));
	}

public:
	double lastShot;
	mat4 playerMatrix;
	int lives;
};