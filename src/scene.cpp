#include "scene.h"
#include "utils.h"
#include "extra\hdre.h"
#include "input.h"
#include <fstream>
#include <map>
#include <iomanip>

Scene::Scene(Camera* camera) {
	this->camera = camera;
	track = new Entity();
	
};

bool Scene::parseScene(const char* filename, Shader* shader)
{
	// You could fill the map manually to add shader and texture for each mesh
	// If the mesh is not in the map, you can use the MTL file to render its colors
	//meshes_to_load["meshes/example.obj"] = { Texture::Get("texture.tga"), Shader::Get("shader.vs", "shader.fs") };
	meshes_to_load.clear();
	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}


	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, anything to do here
		if (render_data.models.empty())
			continue;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			//Esto deberia ser un instancedPrefabEntity o algo asi
			PrefabEntity* new_entity = new PrefabEntity(mesh_name.c_str(), Mesh::Get(mesh_name.c_str()), shader, render_data.texture);
			// Add all instances
			//new_entity->models = render_data.models;
			// Add entity to scene root
			track->addChild(new_entity);
		}
		// Create normal entity
		else {
			PrefabEntity* new_entity = new PrefabEntity(mesh_name.c_str(), Mesh::Get(mesh_name.c_str()), shader, render_data.texture);
			new_entity->model = render_data.models[0];
			new_entity->layer = TRACK;
			// Add entity to scene root
			track->addChild(new_entity);
		}
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

bool Scene::parseCar(const char* filename, CarEntity* car ,Shader* shader)
{
	// You could fill the map manually to add shader and texture for each mesh
	// If the mesh is not in the map, you can use the MTL file to render its colors
	//meshes_to_load["meshes/example.obj"] = { Texture::Get("texture.tga"), Shader::Get("shader.vs", "shader.fs") };
	meshes_to_load.clear();
	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}


	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, anything to do here
		if (render_data.models.empty())
			continue;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			//Esto deberia ser un instancedPrefabEntity o algo asi
			PrefabEntity* new_entity = new PrefabEntity(mesh_name.c_str(), Mesh::Get(mesh_name.c_str()), shader, render_data.texture);
			// Add all instances
			//new_entity->models = render_data.models;
			// Add entity to scene root
			car->addChild(new_entity);
		}
		// Create normal entity
		else {
			PrefabEntity* new_entity = new PrefabEntity(mesh_name.c_str(), Mesh::Get(mesh_name.c_str()), shader, render_data.texture);
			new_entity->model = render_data.models[0];
			// Add entity to scene root
			car->addChild(new_entity);
		}
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}


PlayScene::PlayScene(Camera* camera) : Scene(camera) {
	car = nullptr;
	angle = 0;
	mouse_speed = 10.0f;
	mouse_locked = false;
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phongobj.fs");
	skybox = CubemapFromHDRE("data/panorama.hdre");
}


void PlayScene::setupScene(int window_width, int window_height) {
	// example of shader loading using the shaders manager
	
	parseScene("data/track.scene", shader);
	//parseScene("data/aaa/paul/paul.scene", shader);

	//speed values
	//los parametros se pasan dentro de dos structs para tener constructores mas sencillos
	sSpeedParameters sp = sSpeedParameters();
	sp.max_speed = 60;
	sp.max_acceleration = 10.5f;
	sp.max_bracking = 20.5;
	sp.downforce = 2.2;

	sTurningParameters tp = sTurningParameters();
	tp.max_angle = 30;
	tp.turning_acceleration = 4;
	tp.max_turn_speed = 1.1f;
	tp.centering_acceleration = 0.1;
	tp.turning_speed_mult = 1.4f;

	car = new CarEntity(
		"car", 
		Vector3(200, 3, 500),
		shader,
		sp,
		tp,
		0.4
	);

	parseCar("data/car.scene", car ,shader);

	//track = new PrefabEntity("track", Vector3(1, 1, 1), "data/track.obj", "data/grass.png", shader);

	Vector3 car_pos = car->model.getTranslation();

	//create our first person camera
	//camera->lookAt(Vector3(car_pos.x, car_pos.y + 3.6, car_pos.z), Vector3(car_pos.x, car_pos.y + 3.39, car_pos.z + 1), Vector3(0.f, 1.f, 0.f));
	//camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	//create our third person camera	
	camera->lookAt(Vector3(car_pos.x, car_pos.y + 3, car_pos.z - 4), Vector3(car_pos.x, car_pos.y + 3, car_pos.z + 1), Vector3(0.f, 1.f, 0.f));
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	t.start();
}

std::string formatTime(long long milliseconds) {
	int totalSeconds = static_cast<int>(milliseconds / 1000);
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	int millisecondsRemainder = milliseconds % 1000;

	// Construct the formatted time string
	std::stringstream ss;
	if (minutes > 0) {
		ss << std::setw(1) << std::setfill('0') << minutes << ":";
	}

	ss << std::setw(2) << std::setfill('0') << seconds << "." << std::setw(3) << std::setfill('0') << millisecondsRemainder;

	return ss.str();
}

void PlayScene::renderScene() {
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (!shader)
		return;

	generateSkybox();
	//root renderiza la pista
	track->render(camera);
	//renderiza el coche
	car->render(camera);

	//Draw the floor grid
	//drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	//TODO hacer que la velocidad se muestre en "KPH"
	drawText(370, 530, std::to_string((int)car->getSpeed()*6), Vector3(1, 1, 1), 2);
	drawText(370, 550, car->getGear(), Vector3(1, 1, 1), 2);
	drawText(370, 570, std::to_string(car->getRotationSpeed()), Vector3(1, 1, 1), 2);
	drawText(370, 510, "Time: " + formatTime(t.getTime()), Vector3(1, 1, 1), 2);
}

void PlayScene::update(float dt) {
	float speed = dt * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

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
		turn--;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_R) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		car->goBackwards();
	}
	if (Input::isKeyPressed(SDL_SCANCODE_F) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		car->goForwards();
	}
	if (Input::isKeyPressed(SDL_SCANCODE_P)) {
		t.stop();
	}
	car->move(dir, turn, dt, camera);
	std::vector<sCollisionData> collisions = std::vector<sCollisionData>();
	checkCarCollisions(collisions);
	Vector3 current_car_pos = car->getPosition();
	if(!collisions.empty())
		car->model.setPosition(current_car_pos.x, collisions[0].colPoint.y+1, current_car_pos.z);

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

bool PlayScene::checkCarCollisions(std::vector<sCollisionData>& collisions) {
	float sphereRadius = 3.f;
	Vector3 colPoint, colNormal;

	// For each collider entity “e” in root:
	for each (PrefabEntity* e in track->children)
	{
		Mesh* mesh = e->mesh;

		if (mesh->testRayCollision(e->model, car->getPosition() , Vector3(0, -1, 0), colPoint, colNormal, 50.f, false)) {
			if (e->layer & TRACK) {
				collisions.push_back({ colPoint,colNormal.normalize() });
			}
		}
	}
	
	// End loop

	return !collisions.empty();

}