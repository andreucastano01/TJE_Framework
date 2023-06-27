#pragma once

#include <vector>
#include "entity.h"
#include "camera.h"
#include "shader.h"
#include "timer.h"
#include "extra/bass.h"
#include "UI.h"

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
	Entity* track;
	timer t;
	Texture* skybox;
	int window_width;
	int window_height;
	std::map<std::string, sRenderData> meshes_to_load;
	Scene(Camera* camera);
	virtual void renderScene(){}
	virtual void update(float dt){}
	bool parseScene(const char* filename, Shader* shader);
	bool parseCar(const char* filename,CarEntity* car, Shader* shader);
	void generateSkybox();
};

class PlayScene : public Scene 
{
public:
	CarEntity* car;
	Shader* shader;
	float angle;
	float mouse_speed;
	bool mouse_locked;
	UI* ui;
	PrefabEntity *finnish, *sector1, *sector2;
	long long best_time;
	std::vector<std::string> tiempos;

	HSAMPLE hSample;	// Handler to store one sample	
	HCHANNEL hSampleChannel; // Handler to store one channel

	PlayScene(Camera* camera);

	void setupScene(int window_width, int window_height);
	void renderScene();
	void update(float dt);
	bool checkCarCollisions(std::vector<sCollisionData>& collisions);
	void setupTrackLimits();
	void guardarTiempos();
};

class IntroScene : public Scene {
public:
	Shader* shader;
	UI* ui;
	float radius;
	float angle;

	IntroScene(Camera* camera);
	void setupScene(int window_width, int window_height);
	void renderScene();
	void update(float dt);
};

class GameOverScene : public Scene {
public:
	Shader* shader;
	UI* ui;
	float radius;
	float angle;
	CarEntity* car;

	GameOverScene(Camera* camera);
	void setupScene(int window_width, int window_height);
	void renderScene();
	void update(float dt);
	void stopAudio();
};