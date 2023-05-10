#pragma once
#include "framework.h"
#include "texture.h"
#include "mesh.h"

class Entity {
public:
	Vector3 position;
	Entity();
};

class PrefabEntity : Entity {
public:
	Mesh* mesh;
	Texture* texture;
	PrefabEntity(Vector3 position, const char* meshf, const char* texturef);
};