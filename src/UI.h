#pragma once
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "camera.h"
#include "entity.h"

class UI {
public:
	UI(int window_width, int window_height);
	int window_width;
	int window_height;

	Shader* shader;
	Texture* velocimeter;
	Camera camera2D;

	void drawGUI();
	void drawVelocimeter();
	void drawTime();
	void drawMinimap(CarEntity* car, Entity* track);
};