#pragma once

#include "texture.h"
#include "mesh.h"

class Entity {
public:
	Entity(std::string name);

	std::string name;
	Matrix44 model;

	virtual void render(){}
	virtual void update(float dt){}

	Matrix44 getGlobalMatrix();
};

class PrefabEntity : public Entity {
public:
	Mesh* mesh;
	Texture* texture;
	Vector4 color;

	PrefabEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Vector4 color);
};

class CarEntity : public PrefabEntity {
public:
	
	float speed;
	float acceleration;
	float angle;

	CarEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Vector4 color, float max_speed, float max_angle, float max_acceleration, float max_breacking, float downforce);

private:
	float max_speed;
	float max_angle;
	float max_acceleration;
	float max_breacking;
	float downforce;
};