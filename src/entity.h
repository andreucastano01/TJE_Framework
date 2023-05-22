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
	Shader* shader;
	Vector4 color;

	PrefabEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Shader* shader);
};

class CarEntity : public PrefabEntity {
public:
	
	float speed;
	float angle;
	
	CarEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Shader* shader, float max_speed, float max_angle, float max_acceleration, float max_breacking, float downforce, float rotation_speed);
	//direction: +1 accel forward -1 brake 0 no acceleration
	//turn: +1 Turn left -1 Trun Right 0 no turn
	void move(int direction, int turn, float dt);

	inline float getSpeed() { return speed; }
	inline void goBackwards() { is_reversing = true; }
	inline void goForwards() { is_reversing = false; }

private:
	float max_speed;
	float max_angle;
	float max_acceleration;
	float max_breacking;
	float downforce;
	float rotation_speed;
	bool is_reversing;
};