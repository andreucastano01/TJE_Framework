#pragma once
#include "framework.h"
#include "texture.h"
#include "mesh.h"

class Entity {
public:
	Entity();

	std::string name;
	Matrix44 model;

	virtual void render(){}
	virtual void update(float dt){}

	Matrix44 getGlobalMatrix();
};

class PrefabEntity : Entity {
public:
	Mesh* mesh;
	Texture* texture;
	Shader* shader;
	Vector4 color;

	PrefabEntity(Vector3 position, const char* meshf, const char* texturef, const char* vsf, const char* fsf, Vector4 color);
	void render();
};