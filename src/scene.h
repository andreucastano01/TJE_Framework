#pragma once

#include <vector>
#include "entity.h"
#include "camera.h"
#include "shader.h"
#include "timer.h"

Texture* CubemapFromHDRE(const char* filename);

struct sRenderData {
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	std::vector<Matrix44> models;
};

class Scene
{
public:
	Camera* camera;
	Entity* root;
	timer t;
	std::map<std::string, sRenderData> meshes_to_load;
	Scene(Camera* camera);
	virtual void renderScene(){}
	virtual void update(float dt){}
	bool parseScene(const char* filename, Shader* shader);
	bool parseCar(const char* filename,CarEntity* car, Shader* shader);
};

class PlayScene : public Scene 
{
public:
	CarEntity* car;
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

