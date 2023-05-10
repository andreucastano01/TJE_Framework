#include "entity.h"

Entity::Entity() {
}

PrefabEntity::PrefabEntity(Vector3 position, const char* meshf, const char* texturef) {
	mesh = Mesh::Get(meshf);
	if(texturef) texture = Texture::Get(texturef);
	this->position = position;
}