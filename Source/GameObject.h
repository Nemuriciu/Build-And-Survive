#pragma once
#pragma once
using namespace glm;
using namespace std;

class GameObject
{
public:
	GameObject(){};
	~GameObject(){};
	virtual void drawObject() {};

public:
	vec3 position;
	float sphereRadius;
};