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
	this->is_reversing = false;

	speed = 0;
	angle = 0;
}

void CarEntity::move(int direction, int turn, float dt) {
	//   forwards | backwards
	Vector3 current_pos = model.getTranslation();
	//if reversing make acceleation fo back and break go front
	float flipSides = !is_reversing ? 1 : -1;
	if (direction == 1 * flipSides) {
		speed += max_acceleration * dt;
	}
	else if (direction == -1 * flipSides) {
		speed -= max_breacking * dt;
	}
	else if(direction ==0 && speed != 0) {
		//if no input slowly stop the car

		float deceleration_direction = copysign(1.0, speed);
		speed += -deceleration_direction * downforce * dt;
		if (abs(speed) < 0.01)
			speed = 0;
	}
	float min_value = !is_reversing ? 0 : -2;
	float max_value = !is_reversing ? max_speed : 0;
	speed = clamp(speed, min_value, max_value);

	
	
	model.setTranslation(current_pos.x, current_pos.y, current_pos.z + speed);
}
