#pragma once

#include <vector>
#include "entity.h"
#include "camera.h"
#include "shader.h"

Texture* CubemapFromHDRE(const char* filename);

class Scene
{
public:
	Camera* camera;

	Scene(Camera* camera);
	virtual void renderScene(){}
	virtual void update(float dt){}
};

class PlayScene : public Scene 
{
public:
	
	std::vector<PrefabEntity*> prefab_entities;
	CarEntity* car;
	PrefabEntity* track;
	Shader* shader;
	Texture* skybox;
	float angle;
	float mouse_speed;
	bool mouse_locked;

	PlayScene(Camera* camera);

	void generateSkybox();
	void setupScene(int window_width, int window_height);
	void renderScene();
	void update(float dt);
};

