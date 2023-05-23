#include "scene.h"
#include "utils.h"
#include "extra\hdre.h"
#include "input.h"

Scene::Scene(Camera* camera) {
	this->camera = camera;

};

PlayScene::PlayScene(Camera* camera) : Scene(camera) {
	prefab_entities = std::vector<PrefabEntity*>();
	car = nullptr;
	track = nullptr;
	angle = 0;
	mouse_speed = 10.0f;
	mouse_locked = false;
}
Texture* CubemapFromHDRE(const char* filename)
{
	HDRE* hdre = HDRE::Get(filename);
	if (!hdre)
		return NULL;

	Texture* texture = new Texture();
	if (hdre->getFacesf(0))
	{
		texture->createCubemap(hdre->width, hdre->height, (Uint8**)hdre->getFacesf(0),
			hdre->header.numChannels == 3 ? GL_RGB : GL_RGBA, GL_FLOAT);
		for (int i = 1; i < hdre->levels; ++i)
			texture->uploadCubemap(texture->format, texture->type, false,
				(Uint8**)hdre->getFacesf(i), GL_RGBA32F, i);
	}
	else
		if (hdre->getFacesh(0))
		{
			texture->createCubemap(hdre->width, hdre->height, (Uint8**)hdre->getFacesh(0),
				hdre->header.numChannels == 3 ? GL_RGB : GL_RGBA, GL_HALF_FLOAT);
			for (int i = 1; i < hdre->levels; ++i)
				texture->uploadCubemap(texture->format, texture->type, false,
					(Uint8**)hdre->getFacesh(i), GL_RGBA16F, i);
		}
	return texture;
}

void PlayScene::setupScene(int window_width, int window_height) {

	car = new CarEntity(
		"car", 
		Vector3(1, 3, 1),
		"data/car.obj", 
		"data/Image_13.png", 
		Vector4(1, 1, 1, 1),
		7.0,
		30.0,
		2.5,
		3.5,
		1.2
	);
	prefab_entities.push_back(car);
	track = new PrefabEntity("track", Vector3(1, 1, 1), "data/track.obj", "data/grass.png", Vector4(1, 1, 1, 1));
	prefab_entities.push_back(track);

	skybox = CubemapFromHDRE("data/panorama.hdre");

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	//create our first person camera
	/*
	camera = new Camera();
	camera->lookAt(Vector3(car_pos.x, car_pos.y + 3.6, car_pos.z), Vector3(car_pos.x, car_pos.y + 3.39, car_pos.z + 1), Vector3(0.f, 1.f, 0.f));
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
	*/

	//create our third person camera
	Vector3 car_pos = car->model.getTranslation();
	camera->lookAt(Vector3(car_pos.x, car_pos.y + 6, car_pos.z - 7), Vector3(car_pos.x, car_pos.y + 6, car_pos.z + 1), Vector3(0.f, 1.f, 0.f));
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
}

void PlayScene::renderScene() {
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (!shader)
		return;

	generateSkybox();

	for (PrefabEntity* entity : prefab_entities) {
		//enable shader
		shader->enable();
		
		Vector3 sphere_center = entity->model * entity->mesh->box.center;
		float sphere_radius = entity->mesh->radius;

		// Discard objects whose bounding sphere 
		// is not inside the camera frustum
		if (camera->testSphereInFrustum(sphere_center, sphere_radius) == false)
			continue;

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", entity->texture, 0);
		shader->setUniform("u_model", entity->model);
		

		//do the draw call
		entity->mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
	
	//Draw the floor grid
	//drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	//TODO hacer que la velocidad se muestre en "KPH"
	drawText(370, 550, std::to_string(car->getSpeed()), Vector3(1, 1, 1), 2);
	drawText(370, 570, car->getGear(), Vector3(1, 1, 1), 2);
}

void PlayScene::update(float dt) {
	float speed = dt * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//example

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}
	

	Vector3 car_pos = car->model.getTranslation();
	int dir = 0;
	int turn = 0;

	//async input to move the camera around
	//if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) {
		dir++;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
		dir--;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
		//camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		turn++;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		//camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
		turn--;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_R) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		car->goBackwards();
	}
	if (Input::isKeyPressed(SDL_SCANCODE_F) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		car->goForwards();
	}
	car->move(dir, turn, dt);
			//entity->model.rotate(angle, Vector3(0.0, 1.0, 0.0));
	camera->lookAt(Vector3(car_pos.x, car_pos.y + 6, car_pos.z - 7), Vector3(car_pos.x, car_pos.y + 6, car_pos.z + 1), Vector3(0.f, 1.f, 0.f));
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

void PlayScene::generateSkybox() {
	Mesh* mesh = Mesh::Get("data/sphere.obj");
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/skybox.fs");
	shader->enable();

	Matrix44 model;

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	model.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
	model.scale(5, 5, 5);

	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_texture", skybox, 0);

	mesh->render(GL_TRIANGLES);
	shader->disable();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

