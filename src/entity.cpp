#include "entity.h"
#include "camera.h"
#include "shader.h"

Entity::Entity(std::string name, float scale) {
	this->scale = scale;
	this->name = name;
	parent = nullptr;
}

Entity::Entity() {
	parent = nullptr;
	scale = 1;
}

void Entity::render(Camera* camera) {
	for (int i = 0; i < children.size(); i++) {
		//cast to prefab to render
		PrefabEntity* ent = (PrefabEntity*)children[i];
		if (ent->layer != TRACK) continue;
		ent->render(camera);
	}	
}

void Entity::minimaprender(Camera* camera) {
	for (int i = 0; i < children.size(); i++) {
		//cast to prefab to render
		PrefabEntity* ent = (PrefabEntity*)children[i];
		if (ent->minimap_render == true) ent->render(camera);
	}
}

Matrix44 Entity::getGlobalMatrix() {
	if (parent)
		return model * parent->getGlobalMatrix();

	return model;
}

void Entity::addChild(Entity* child) {
	assert(child->parent == nullptr);
	children.push_back(child);
	child->parent = this;
}

void Entity::removeChild(Entity* child) {
}

PrefabEntity::PrefabEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Shader* shader, float scale) : Entity(name, scale){
	mesh = Mesh::Get(meshf);
	if (texturef) texture = Texture::Get(texturef); else texture = new Texture();
	model.setTranslation(position.x, position.y, position.z);
	this->shader = shader;
	isDynamic = false;
}

PrefabEntity::PrefabEntity(std::string name, Mesh* mesh, Shader* shader, Texture* texture, float scale) : Entity(name, scale) {
	this->mesh = mesh;
	this->shader = shader;
	if (texture) this->texture = texture;
	else this->texture = new Texture();
	model.setIdentity();
	isDynamic = false;
}

PrefabEntity::PrefabEntity(std::string name, Vector3 position, Shader* shader, float scale) : Entity(name, scale) {
	mesh = nullptr;
	texture = nullptr;
	model.setTranslation(position.x, position.y, position.z);
	this->shader = shader;
	isDynamic = false;
}

void PrefabEntity::render(Camera* camera) {

	const Matrix44& globalMatrix = getGlobalMatrix();
	Vector3 box_center = globalMatrix * mesh->box.center;
	Vector3 half_size = mesh->box.halfsize;

	//mesh->renderBounding(globalMatrix);

	// Discard objects whose bounding sphere 
	// is not inside the camera frustum
	if (camera->testBoxInFrustum(box_center, half_size) == false) {
		//std::cout << "a" << std::endl;
		return;
	}
	

	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
	shader->enable();
	

	//upload uniforms
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_model", globalMatrix);
	shader->setUniform("lightPos", Vector3(10, 10, 10));
	shader->setUniform("Ia", 0.7f);
	shader->setUniform("Kd", 0.9f);
	shader->setUniform("Ks", 0.4f);

	//do the draw call
	mesh->render(GL_TRIANGLES);

	//disable shader
	shader->disable();
	
	for (int i = 0; i < children.size(); i++)
		children[i]->render(camera);
}

CarEntity::CarEntity(std::string name, Vector3 position, Shader* shader, sSpeedParameters sp, sTurningParameters tp, float scale) : PrefabEntity(name, position, shader, scale) {
	speedParams = sp;
	turnParams = tp;
	isDynamic = true;
	sectors[0] = false;
	sectors[1] = false;

	this->is_reversing = false;
	this->model.scale(0.5,0.5,0.5);
	speed = 0;
	angle = 0;
	rotation_speed = 0;
}

void CarEntity::move(int direction, int turn, float dt, Camera* camera, bool cameraView) {
	//   forwards | backwards
	Vector3 current_pos = model.getTranslation();
	Matrix44 current_rot = model.getRotationOnly();
	//rename for simplicity
	sSpeedParameters sp = speedParams;
	sTurningParameters tp = turnParams;
	//if reversing make acceleation fo back and break go front
	float flipSides = !is_reversing ? 1 : -1;
	if (direction == 1 * flipSides) {
		speed += (sp.max_acceleration - sp.downforce - min(0,speed/sp.downforce)) * dt;
	}
	else if (direction == -1 * flipSides) {
		speed -= (sp.max_bracking + sp.downforce) * dt;
	}
	else if(direction == 0 && speed != 0) {
		//if no input slowly stop the car

		float deceleration_direction = copysign(1.0, speed);
		speed += -deceleration_direction * sp.downforce*2 * dt;
		if (abs(speed) < 0.01)
			speed = 0;
	}

	float min_value = !is_reversing ? 0 : -2;
	float max_value = !is_reversing ? sp.max_speed : 0;
	speed = clamp(speed, min_value, max_value);

	//Rotation
	if (turn == 0) {
		if (rotation_speed > 0.001) {
			rotation_speed -= tp.turning_acceleration * dt;
		}
		else if (rotation_speed < -0.001) {
			rotation_speed += tp.turning_acceleration * dt;
		}
		else {
			rotation_speed = 0;
		}
		
	}
	else 
		rotation_speed += turn*tp.turning_acceleration * dt;

	float max_rotation_speed;
	float normalized_speed = speed / sp.max_speed;
	if (speed == 0.0) {
		max_rotation_speed = 0;
	}
	else {
		max_rotation_speed = tp.max_turn_speed + tp.turning_speed_mult*(1-normalized_speed);
	}

	rotation_speed = clamp(rotation_speed, -max_rotation_speed, max_rotation_speed);

	float rotation_amount =  rotation_speed;
	rotation_amount =  clamp(rotation_amount, -tp.max_angle, tp.max_angle);
	angle += rotation_amount*dt;
	

	Matrix44 rotation_matrix;
	rotation_matrix.setRotation(angle, Vector3(0, -1, 0));
	Vector3 forward = rotation_matrix * Vector3(0, 0, 1);
	current_pos.x += forward.x * (speed * dt);
	current_pos.y += forward.y * (speed * dt);
	current_pos.z += forward.z * (speed * dt);
	
	model.setTranslation(current_pos.x, current_pos.y, current_pos.z);
	model.scale(scale, scale, scale);
	model.rotate(angle, Vector3(0, -1, 0));

	//Camera
	Vector3 camera_offset;
	if (cameraView)
		camera_offset = Vector3(0, 0.79, -0.2f); //First person
	else
		camera_offset = Vector3(0, 10, -10.f); //Third person

	Vector3 rotated_offset = rotation_matrix * camera_offset;
	Vector3 camera_pos = current_pos + rotated_offset;
	camera->eye = camera_pos;
	if(cameraView)
		camera->center = Vector3(current_pos.x, current_pos.y + 0.76, current_pos.z); //First person
	else
		camera->center = Vector3(current_pos.x, current_pos.y + 3, current_pos.z + 1); //Third person
	
}

void CarEntity::render(Camera* camera) {
	for (int i = 0; i < children.size(); i++) {
		PrefabEntity* ent = (PrefabEntity*)children[i];
		//Matrix44 m = ent->model * model;
		//ent->model = ent->getGlobalMatrix();
		ent->render(camera);
	}
}
