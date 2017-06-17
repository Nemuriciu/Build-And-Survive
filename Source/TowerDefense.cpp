#pragma once
#include <iostream>
#include <Core/Engine.h>
#include <TowerDefense.h>
using namespace std;
using namespace glm;

TowerDefense::TowerDefense(){}
TowerDefense::~TowerDefense(){}

float FOVY = RADIANS(60);
float FOVX = RADIANS(60);

void TowerDefense::Init()
{
	TPCamera = new GameCamera();
	FPCamera = new GameCamera();
	TPCamera->Set(vec3(0, 1.5, -5) , vec3(0, -1.35, 0), vec3(0, 1, 0));
	FPCamera->Set(vec3(0, 1, -5), vec3(0, -1, 0), vec3(0, 1, 0));
	FPCamera->distanceToTarget = -1;

	// Initialize Tower Mesh + Spawn 3 towers
	Mesh *towerMesh = new Mesh("Tower");
	towerMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "Tower.obj");
	meshes[towerMesh->GetMeshID()] = towerMesh;
	towers.push_back(new Tower(vec3(-2.5, 0, 0)));
	towers.push_back(new Tower(vec3(2.5, 0, 0)));
	towers.push_back(new Tower(vec3(0, 0, 2.5)));

	// Initialize Enemy Mesh
	Mesh *enemyMesh = new Mesh("Enemy");
	enemyMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "Spectre.obj");
	meshes[enemyMesh->GetMeshID()] = enemyMesh;

	// Initialize Floor Mesh
	Mesh *floorMesh = new Mesh("Floor");
	floorMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
	meshes[floorMesh->GetMeshID()] = floorMesh;

	// Initialize Player
	player = new Player();
	Mesh *playerMesh = new Mesh("Player");
	playerMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "ArmyPilot.obj");
	meshes[playerMesh->GetMeshID()] = playerMesh;
	player->lives = 3;

	// Initialize Player Lives
	Mesh *lifeMesh = new Mesh("LifeSphere");
	lifeMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	meshes[lifeMesh->GetMeshID()] = lifeMesh;

	LifeSphere* life = new LifeSphere();
	life->angle = RADIANS(120);
	playerLives.push_back(new LifeSphere());
	playerLives.push_back(life);
	life = new LifeSphere();
	life->angle = RADIANS(240);
	playerLives.push_back(life);

	// Initialize Bullet Meshes
	Mesh *bulletMesh = new Mesh("Bullet");
	bulletMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	meshes[bulletMesh->GetMeshID()] = bulletMesh;

	// Minimap Cubes
	Mesh *cube = new Mesh("Box");
	cube->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
	meshes[cube->GetMeshID()] = cube;

	// Create MyShader
	Shader *shader = new Shader("MyShader");
	shader->AddShader("Resources/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	shader->AddShader("Resources/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	shaders[shader->GetName()] = shader;

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void TowerDefense::FrameStart()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void TowerDefense::Update(float deltaTimeSeconds)
{	
	if (player->lives <= 0)
	{
		color += 0.025f;
		glClearColor(color, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		// Get Player Position
		if (isFirstPerson)
		{
			player->position = FPCamera->GetTargetPosition();
			player->position.y -= 1;
		}
		else
		{
			player->position = TPCamera->GetTargetPosition();
		}

		// Draw Floor
		floorMatrix = glm::mat4(1);
		floorMatrix = glm::translate(floorMatrix, vec3(0, 0, 0));
		floorMatrix = glm::scale(floorMatrix, vec3(0.5f));
		RenderMesh(meshes["Floor"], shaders["VertexColor"], floorMatrix);

		// Draw Player
		player->drawObject(angle);
		RenderMesh(meshes["Player"], shaders["MyShader"], player->playerMatrix);

		// Draw Player Lives
		for (int i = 0; i < player->lives; i++)
		{
			playerLives[i]->drawObject(player->position);
			RenderMesh(meshes["LifeSphere"], shaders["MyShader"], playerLives[i]->sphereMatrix);
		}

		// Draw Towers
		for (int i = 0; i < towers.size(); i++)
		{
			Tower *current = towers[i];
			current->drawObject();
			RenderMesh(meshes["Tower"], shaders["MyShader"], current->towerMatrix);
		}

		// Draw Enemy
		for (int i = 0; i < enemies.size(); i++)
		{
			Enemy* current = enemies[i];
			if (enemies[i]->isDead || enemies[i]->finished)
			{
				if (enemies[i]->finished) player->lives--;
				vector<Enemy*>::iterator position = find(enemies.begin(), enemies.end(), enemies[i]);
				int pos = std::distance(enemies.begin(), position);
				enemies.erase(enemies.begin() + pos);
				i--;
				continue;
			}

			current->drawObject();
			RenderMesh(meshes["Enemy"], shaders["MyShader"], current->enemyMatrix);

			/* Tower -> Shoot at Enemy if in range and not Dying */
			if (!current->isDying)
			{
				for (int i = 0; i < towers.size(); i++)
				{
					Tower* tower = towers[i];
					if (isCollision(current, tower))
					{
						tower->angle = atan2(current->position.x - tower->position.x,
							current->position.z - tower->position.z);
						double time = Engine::GetElapsedTime();
						if (time - towers[i]->lastShot >= 1.0)
						{
							tower->bullet = new Bullet(1, towers[i]->position);
							tower->bullet->position.y += 1;
							tower->bullet->fixedPosition = current->position;
							tower->bullet->fixedPosition.y += 1;
							tower->lastShot = time;
						}
					}
				}
			}
		}

		// Create Player Bullets
		// TODO: CHECK MULTIPLE FASt CLICKS
		if (mouseClick)
		{
			if (weapon == 1)
			{
				double time = Engine::GetElapsedTime();
				if (time - player->lastShot >= 1.5f)
				{
					Bullet* bullet = new Bullet(1, player->position);
					bullet->speed = 0.15f;
					bullet->angle = angle;
					bullets.push_back(bullet);
					player->lastShot = time;
					mouseClick = false;
				}
			}
			else if (weapon == 2)
			{
				double time = Engine::GetElapsedTime();
				if (time - player->lastShot >= 0.5f)
				{
					Bullet* bullet = new Bullet(2, player->position);
					bullet->speed = 0.35f;
					bullet->angle = angle;
					bullets.push_back(bullet);
					player->lastShot = time;
					mouseClick = false;
				}
			}
			else if (weapon == 3)
			{
				double time = Engine::GetElapsedTime();
				if (time - player->lastShot >= 2.5f)
				{
					Bullet* bullet = new Bullet(3, player->position);
					bullet->speed = 0.05f;
					bullet->angle = angle;
					bullets.push_back(bullet);
					player->lastShot = time;
					mouseClick = false;
				}
			}
		}

		// Draw Player Bullets
		for (int i = 0; i < bullets.size(); i++)
		{
			Bullet* current = bullets[i];
			current->drawObject(false);

			if(current->bulletSize == 1)
			{
				if (current->distTraveled <= 7.5f)
					RenderMesh(meshes["Bullet"], shaders["MyShader"], current->bulletMatrix);
				else
				{
					vector<Bullet*>::iterator position = find(bullets.begin(), bullets.end(), current);
					int pos = std::distance(bullets.begin(), position);
					bullets.erase(bullets.begin() + pos);
					i--;
				}
			}
			if (current->bulletSize == 2)
			{
				if (current->distTraveled <= 25.0f)
					RenderMesh(meshes["Bullet"], shaders["MyShader"], current->bulletMatrix);
				else
				{
					vector<Bullet*>::iterator position = find(bullets.begin(), bullets.end(), current);
					int pos = std::distance(bullets.begin(), position);
					bullets.erase(bullets.begin() + pos);
					i--;
				}
			}
			else if(current->bulletSize == 3)
			{
				if (current->position.y >= 0)
					RenderMesh(meshes["Bullet"], shaders["MyShader"], current->bulletMatrix);
				else
				{
					vector<Bullet*>::iterator position = find(bullets.begin(), bullets.end(), current);
					int pos = std::distance(bullets.begin(), position);
					bullets.erase(bullets.begin() + pos);
					i--;
				}
			}
		}

		// Draw Tower Bullets
		for (int i = 0; i < towers.size(); i++)
		{
			Tower* current = towers[i];
			if (current->bullet != NULL)
			{
				current->bullet->drawObject(true);
				RenderMesh(meshes["Bullet"], shaders["MyShader"], current->bullet->bulletMatrix);
			}
		}

		// Check Player Bullet <-> Enemy Collision
		for (int i = 0; i < bullets.size(); i++)
		{
			for (int j = 0; j < enemies.size(); j++)
			{
				if (!enemies[j]->isDying)
				{
					if (isCollision(enemies[j], bullets[i]))
					{
						// Delete Bullet
						if (bullets[i]->bulletSize == 1)	enemies[j]->lives -= 2;
						else if (bullets[i]->bulletSize == 2) enemies[j]->lives--;
						else if (bullets[i]->bulletSize == 3) enemies[j]->lives -= 5;

						enemies[j]->scaleSize -= 0.0005f;
						vector<Bullet*>::iterator position = find(bullets.begin(), bullets.end(), bullets[i]);
						int pos = std::distance(bullets.begin(), position);
						bullets.erase(bullets.begin() + pos);
						i--;

						//Delete Enemy
						if (enemies[j]->lives <= 0)
							enemies[j]->isDying = true;
						break;
					}
				}
			}
		}

		// Check Tower Bullet <-> Enemy Collision
		for (int i = 0; i < towers.size(); i++)
		{
			if (towers[i]->bullet == NULL) continue;
			for (int j = 0; j < enemies.size(); j++)
			{
				if (!enemies[j]->isDying)
				{
					if (isCollision(enemies[j], towers[i]->bullet))
					{
						towers[i]->bullet = NULL;
						enemies[j]->lives--;
						enemies[j]->scaleSize -= 0.0005f;

						if (enemies[j]->lives <= 0)
							enemies[j]->isDying = true;
						break;
					}
				}
			}
		}

		if (isFirstPerson)
			DrawCoordinatSystem(FPCamera->GetViewMatrix(), projectionMatrix);
		else
			DrawCoordinatSystem(TPCamera->GetViewMatrix(), projectionMatrix);

		// MINIMAP
		minimap = true;
		minimapCamera = new GameCamera();
		minimapCamera->Set(vec3(0, 5, 0.01), vec3(0, 1, 0), vec3(0, 1, 0));
		glViewport(1000, 50, 200, 200);
		minimapMatrix = glm::ortho(2.5, -2.5, 2.5, -2.5, 0.01, 10.0);
		

		// Minimap Floor
		mat4 floor = glm::mat4(1);
		floor = glm::translate(floor, vec3(0, -3, 0));
		floor = glm::scale(floor, vec3(5.0f));
		floor = glm::rotate(floor, RADIANS(180), vec3(1, 0, 0));
		RenderMesh(meshes["Box"], shaders["MyShader"], floor);

		// Minimap Player
		mat4 pMatrix = glm::mat4(1);
		pMatrix = glm::translate(pMatrix, 
			vec3(player->position.x / 4, player->position.y / 4, player->position.z / 4));
		pMatrix = rotate(pMatrix, angle, glm::vec3(0, 1, 0));
		pMatrix = rotate(pMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
		pMatrix = scale(pMatrix, glm::vec3(0.25f));
		RenderMesh(meshes["Box"], shaders["MyShader"], pMatrix);

		// Minimap Towers
		for (int i = 0; i < towers.size(); i++)
		{
			Tower *current = towers[i];
			mat4 tMatrix = glm::mat4(1);
			tMatrix = glm::translate(tMatrix, 
				vec3(current->position.x / 4, current->position.y / 4, current->position.z / 4));
			tMatrix = scale(tMatrix, glm::vec3(0.25f));
			RenderMesh(meshes["Box"], shaders["MyShader"], tMatrix);
		}

		// Minimap Enemies
		for (int i = 0; i < enemies.size(); i++)
		{
			Enemy *current = enemies[i];
			mat4 eMatrix = glm::mat4(1);
			eMatrix = glm::translate(eMatrix,
				vec3(current->position.x / 5, current->position.y / 5, current->position.z / 5));
			eMatrix = scale(eMatrix, glm::vec3(0.25f));
			eMatrix = rotate(eMatrix, RADIANS(90), glm::vec3(0, 0, 1));
			RenderMesh(meshes["Box"], shaders["MyShader"], eMatrix);
		}

		minimap = false;
	}
}

void TowerDefense::FrameEnd()
{
	/*
	if (isFirstPerson)
		DrawCoordinatSystem(FPCamera->GetViewMatrix(), projectionMatrix);
	else
		DrawCoordinatSystem(TPCamera->GetViewMatrix(), projectionMatrix);
	*/
}

void TowerDefense::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader)
		return;
	shader->Use();
	if (!minimap)
	{
		if (isFirstPerson)
			glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(FPCamera->GetViewMatrix()));
		else
			glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(TPCamera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(projectionMatrix));
	}
	else
	{
		glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(minimapCamera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(minimapMatrix));		
	}
	
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	mesh->Render();
}

bool TowerDefense::isCollision(GameObject* obj1, GameObject* obj2)
{		
	/* Collision with Enemy */
	if (dynamic_cast<Enemy*> (obj1) != NULL)
	{
		float deltaX = obj1->position.x - obj2->position.x;
		float deltaY = obj1->position.y - obj2->position.y;
		float deltaZ = obj1->position.z - obj2->position.z;
		deltaX *= deltaX;
		deltaY *= deltaY;
		deltaZ *= deltaZ;

		float radius = obj1->sphereRadius + obj2->sphereRadius;
		radius *= radius;

		if (deltaX + deltaY + deltaZ <= radius)
		{
			/* Tower <-> Enemy */
			if (dynamic_cast<Tower*> (obj2) != NULL)
				return true;
			/* Bullet <-> Enemy */
			else if(dynamic_cast<Bullet*> (obj2) != NULL)
			{
				return true;
			}
		}
	}
	return false;
};

void TowerDefense::OnInputUpdate(float deltaTime, int mods)
{
	if (player->lives > 0)
	{
		if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
		{
			float cameraSpeed = 0.05f;

			if (window->KeyHold(GLFW_KEY_W))
			{
				TPCamera->MoveForward(cameraSpeed);
				FPCamera->MoveForward(cameraSpeed);
			}

			if (window->KeyHold(GLFW_KEY_A))
			{
				TPCamera->TranslateRight(-cameraSpeed);
				FPCamera->TranslateRight(-cameraSpeed);
			}

			if (window->KeyHold(GLFW_KEY_S))
			{
				TPCamera->MoveForward(-cameraSpeed);
				FPCamera->MoveForward(-cameraSpeed);
			}

			if (window->KeyHold(GLFW_KEY_D))
			{
				TPCamera->TranslateRight(cameraSpeed);
				FPCamera->TranslateRight(cameraSpeed);
			}
		}

		double currentTime = Engine::GetElapsedTime();
		if (currentTime - lastTime >= 3.0)
		{
			Enemy* enemy = new Enemy(vec3(10, 0, 10));
			enemy->lives = rand() % 10 + 1;
			enemies.push_back(enemy);
			lastTime = currentTime;
			cout << "*NEW MONSTER* - HP := " << enemy->lives << endl << endl;
		}
	}
}

void TowerDefense::OnKeyPress(int key, int mods)
{
	switch (key)
	{
		case GLFW_KEY_1:
			weapon = 1;
			isFirstPerson = false;
			break;
		case GLFW_KEY_2:
			weapon = 2;
			isFirstPerson = true;
			break;
		case GLFW_KEY_3:
			weapon = 3;
			isFirstPerson = false;
			break;
		default:
			break;
	}
}

void TowerDefense::OnKeyRelease(int key, int mods)
{
}

void TowerDefense::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (player->lives > 0)
	{
		if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
		{
			angle += -deltaX * sensivityOX;
			TPCamera->RotateThirdPerson_OX(-deltaX * sensivityOX);
			FPCamera->RotateThirdPerson_OX(-deltaX * sensivityOX);
		}
	}
}

void TowerDefense::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if(button == 1)
		mouseClick = true;
}

void TowerDefense::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void TowerDefense::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void TowerDefense::OnWindowResize(int width, int height)
{
}