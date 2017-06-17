#pragma once
using namespace glm;

class LifeSphere : public GameObject
{
public:
	LifeSphere(){};
	~LifeSphere(){};

	void drawObject(vec3 pos)
	{
		sphereMatrix = mat4(1);
		sphereMatrix = translate(sphereMatrix, vec3(pos.x, pos.y + 0.5, pos.z));
		sphereMatrix = translate(sphereMatrix, vec3(0, 0, 0));
		sphereMatrix = rotate(sphereMatrix, angle, glm::vec3(0, 1, 0));
		sphereMatrix = translate(sphereMatrix, vec3(0.5, 0, 0));
		sphereMatrix = scale(sphereMatrix, glm::vec3(0.1f));
		angle -= 0.05;
	}

public:
	mat4 sphereMatrix;
	Mesh* mesh;
	int lives;
	float angle = 0;
};