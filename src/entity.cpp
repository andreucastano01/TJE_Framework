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

CarEntity::CarEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Vector4 color, 
	float max_speed, float max_angle, float max_acceleration, float max_breacking, float downforce) : PrefabEntity(name, position, meshf, texturef, color) {
	this->max_speed = max_speed;
	this->max_angle = max_angle;
	this->max_acceleration = max_acceleration;
	this->max_breacking = max_breacking;
	this->downforce = downforce;

	acceleration = 0;
	speed = 0;
	angle = 0;
}
