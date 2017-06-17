#pragma once
#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include <GameCamera.h>
#include <GameObject.h>
#include <Player.h>
#include <Bullet.h>
#include <Enemy.h>
#include <Tower.h>
#include <LifeSphere.h>
using namespace glm;

class TowerDefense : public SimpleScene
{
public:
	TowerDefense();
	~TowerDefense();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	bool TowerDefense::isCollision(GameObject* obj1, GameObject* obj2);
	void RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix) override;
	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

protected:
	Player* player;
	vector<Enemy*> enemies;
	vector<Bullet*> bullets;
	vector<Tower*> towers;
	vector<LifeSphere*> playerLives;
	mat4 projectionMatrix;
	mat4 modelMatrix;
	mat4 floorMatrix;
	mat4 lightMatrix;
	mat4 minimapMatrix;

	GameCamera *TPCamera;
	GameCamera *FPCamera;
	GameCamera *minimapCamera;
	float color = 0;
	bool renderCameraTarget, mouseClick;	
	float sensivityOX = 0.001f;
	float sensivityOY = 0.001f;
	float angle = 0;
	double lastTime = 4;
	int weapon = 1;
	bool isFirstPerson;
	bool minimap;
};