#pragma once

#include "texture.h"
#include "mesh.h"
#include "camera.h"

struct sSpeedParameters {
	float max_speed;
	float max_acceleration;
	float max_bracking;
	float downforce;
};

struct sTurningParameters {
	float max_angle;
	float turning_acceleration;
	float max_turn_speed;
	float centering_acceleration; //speed that turn goes back to 0
	float turning_speed_mult; //when the car goes slower, it turns more
};

enum {
	NONE = 0,
	CAR,
	TRACK,
	WALLS,
	FINISH,
	SECTOR,
	TRACK_LIMITS,
	SCENARIO = TRACK | WALLS,
	ALL = SCENARIO | CAR | FINISH | SECTOR | TRACK_LIMITS,
	
};

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};


class Entity {
public:
	Entity();
	Entity(std::string name, float scale = 1);
	
	std::string name;
	Matrix44 model;
	float scale;
	// Pointer to parent
	Entity* parent;

	// Pointers to children
	std::vector<Entity*> children;

	void addChild(Entity* child);
	void removeChild(Entity* child);
	void render(Camera* camera);
	virtual void update(float dt){}

	Matrix44 getGlobalMatrix();
	inline Vector3 getPosition() { return model.getTranslation(); }
};

class EntityCollider {
public:
	bool isDynamic = false;
	int layer = NONE;
	virtual bool  testCollision(Entity* entity) { return false; }
};

class PrefabEntity : public Entity, public EntityCollider {
public:
	Mesh* mesh;
	Texture* texture;
	Shader* shader;
	Vector4 color;

	PrefabEntity(std::string name, Vector3 position, const char* meshf, const char* texturef, Shader* shader, float scale = 1);
	PrefabEntity(std::string name, Mesh* mesh, Shader* shader, Texture* texture, float scale = 1);
	PrefabEntity(std::string name, Vector3 position, Shader* shader, float scale = 1);

	virtual void render(Camera* camera);
};

class CarEntity : public PrefabEntity{
public:
	
	float speed;
	float angle;
	float rotation_speed;
	
	CarEntity(std::string name, Vector3 position, Shader* shader, sSpeedParameters speedParams, sTurningParameters tp, float scale = 1);
	//direction: +1 accel forward -1 brake 0 no acceleration
	//turn: +1 Turn left -1 Trun Right 0 no turn
	void move(int direction, int turn, float dt, Camera* camera);
	void render(Camera* camera);

	inline float getSpeed() { return speed; }
	inline void goBackwards() { is_reversing = true; }
	inline void goForwards() { is_reversing = false; }
	inline std::string getGear() { return is_reversing ? "R": "1"; }
	inline float getRotationSpeed() { return rotation_speed; }

private:
	sSpeedParameters speedParams;
	sTurningParameters turnParams;

	bool is_reversing;
};


