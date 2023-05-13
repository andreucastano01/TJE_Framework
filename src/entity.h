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