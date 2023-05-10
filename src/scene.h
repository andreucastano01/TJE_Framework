#pragma once

#include "entity.h"
#include <vector>

class Scene
{
public:
	virtual void renderScene(){}
	virtual void update(){}
};

class PlayScene : public Scene 
{
public:
	
	std::vector<PrefabEntity*> entities;
	PrefabEntity* player;

	PlayScene();

	void setupScene();
	void renderScene();
	void update();
};

