#include "entity.h"
#include "camera.h"
#include "shader.h"

Entity::Entity(std::string name) {
	this->name = name;
}

Entity::Entity() {
}

Matrix44 Entity::getGlobalMatrix() {
	if (parent)
		return model * parent->getGlobalMatrix();

	return model;
}

void Entity::addChild(Entity* child) {
	assert(child->parent == NULL);
	children.push_back(child);
	child->parent = this;
}

void Entity::removeChild(Entity* child) {
}

PrefabEntity::PrefabEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Shader* shader) : Entity(name){
	mesh = Mesh::Get(meshf);
	if (texturef) texture = Texture::Get(texturef); else texture = new Texture();
	model.setTranslation(position.x, position.y, position.z);
}

PrefabEntity::PrefabEntity(std::string name, Mesh* mesh, Shader* shader, Texture* texture) : Entity(name) {
	this->mesh = mesh;
	this->shader = shader;
	this->texture = texture;
}

CarEntity::CarEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Shader* shader,
	float max_speed, float max_angle, float max_acceleration, float max_breacking, float downforce, float rotation_speed) : PrefabEntity(name, position, meshf, texturef, shader) {
	this->max_speed = max_speed;
	this->max_angle = max_angle;
	this->max_acceleration = max_acceleration;
	this->max_breacking = max_breacking;
	this->downforce = downforce;
	this->rotation_speed = rotation_speed;

	speed = 0;
	angle = 0;
}

void CarEntity::move(int direction, int turn, float dt, Camera* camera) {
	//   forwards | backwards
	Vector3 current_pos = model.getTranslation();
	Matrix44 current_rot = model.getRotationOnly();

	float flipSides = !is_reversing ? 1 : -1;
	if (direction == 1 * flipSides) {
		speed += max_acceleration * dt;
	}
	else if (direction == -1 * flipSides) {
		speed -= max_breacking * dt;
	}
	else if(direction == 0 && speed != 0) {
		//if no input slowly stop the car

		float deceleration_direction = copysign(1.0, speed);
		speed += -deceleration_direction * downforce * dt;
		if (abs(speed) < 0.01)
			speed = 0;
	}

	float min_value = !is_reversing ? 0 : -2;
	float max_value = !is_reversing ? max_speed : 0;
	speed = clamp(speed, min_value, max_value);

	//Rotation
	float rotation_amount = turn * rotation_speed * dt;
	angle += rotation_amount;

	Matrix44 rotation_matrix;
	rotation_matrix.setRotation(angle, Vector3(0, -1, 0));
	Vector3 forward = rotation_matrix * Vector3(0, 0, 1);
	current_pos.x += forward.x * (speed * dt);
	current_pos.y += forward.y * (speed * dt);
	current_pos.z += forward.z * (speed * dt);
	
	model.setTranslation(current_pos.x, current_pos.y, current_pos.z);
	model.rotate(angle, Vector3(0, -1, 0));

	//Camera
	Vector3 camera_offset = Vector3(0, 6, -7.f); //Third person
	//Vector3 camera_offset = Vector3(0, 3.39, -1.f); //First person
	Vector3 rotated_offset = rotation_matrix * camera_offset;
	Vector3 camera_pos = current_pos + rotated_offset;
	camera->eye = camera_pos;
	camera->center = Vector3(current_pos.x, current_pos.y + 6, current_pos.z + 1); //Third person
	//camera->center = Vector3(current_pos.x, current_pos.y + 3.36, current_pos.z); //First person
}
