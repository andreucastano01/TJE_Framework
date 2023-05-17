#pragma once

#include <vector>
#include "entity.h"
#include "camera.h"
#include "shader.h"

class Scene
{
public:
	virtual void renderScene(Camera* camera){}
	virtual void update(float dt, Camera* camera){}
};

class PlayScene : public Scene 
{
public:
	
	std::vector<PrefabEntity*> prefab_entities;
	PrefabEntity* player;
	PrefabEntity* track;
	Shader* shader;
	Texture* skybox;
	float angle;
	float mouse_speed;
	bool mouse_locked;

	PlayScene();

	void generateSkybox(Camera* camera);
	void setupScene(int window_width, int window_height);
	void renderScene(Camera* camera);
	void update(float dt, Camera* camera);
};

