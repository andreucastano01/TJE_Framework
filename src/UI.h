#pragma once
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "camera.h"
#include "entity.h"

struct Button {
	float x, y;
	float width, height;
	bool isClicked;

	Button(float _x, float _y, float _width, float _height)
		: x(_x), y(_y), width(_width), height(_height), isClicked(false) {}
};

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
	void addButton(float x, float y, float width, float height, const char* name);
};