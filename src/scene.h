#pragma once

#include <vector>
#include "entity.h"
#include "camera.h"
#include "shader.h"

class Scene
{
public:
	virtual void renderScene(Camera* camera){}
	virtual void update(float dt){}
};

class PlayScene : public Scene 
{
public:
	
	std::vector<PrefabEntity*> prefab_entities;
	PrefabEntity* player;
	PrefabEntity* track;
	Shader* shader;

	PlayScene();

	void setupScene(int window_width, int window_height);
	void renderScene(Camera* camera);
	void update(float dt);
};

