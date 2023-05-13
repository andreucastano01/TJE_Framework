#include "entity.h"
#include "camera.h"
#include "shader.h"

Entity::Entity(std::string name) {
	this->name = name;
}

Matrix44 Entity::getGlobalMatrix() {
	return Matrix44();
}


PrefabEntity::PrefabEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Vector4 color) : Entity(name){
	mesh = Mesh::Get(meshf);
	if (texturef) texture = Texture::Get(texturef); else texture = new Texture();
	model.setTranslation(position.x, position.y, position.z);

	this->color = color;
}
