#include <ctime>
#include <iostream>
#include <Core/Engine.h>
#include <TowerDefense.h>
using namespace std;

int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL));
	WindowProperties wp;
	wp.resolution = glm::ivec2(1280, 720);
	WindowObject* window = Engine::Init(wp);
	World *world = new TowerDefense();
	world->Init();
	world->Run();
	Engine::Exit();

	return 0;
}