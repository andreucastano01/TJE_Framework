#include "scene.h"
#include "utils.h"
#include "extra\hdre.h"
#include "input.h"
#include <fstream>
#include <map>
#include <iomanip>
#include "audio.h"
#include <algorithm>

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
			Vector3 nodepos = new_entity->model.getTranslation();
			new_entity->distance_to_camera = nodepos.distance(camera->eye);
			// Add entity to scene root
			car->addChild(new_entity);
		}
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

long long convertirATiempoNumerico(const std::string& tiempo) {
	std::stringstream ss(tiempo);
	char separador;
	int minutos = 0, segundos, milisegundos;

	if (tiempo.find(':') != std::string::npos) {
		ss >> minutos >> separador >> segundos >> separador >> milisegundos;
	}
	else {
		ss >> segundos >> separador >> milisegundos;
	}

	if (ss.fail() || separador != '.' || minutos < 0 || segundos < 0 || segundos >= 60 || milisegundos < 0 || milisegundos >= 1000) {
		std::cerr << "Error al convertir el tiempo" << std::endl;
		return 0;
	}

	long long tiempoNumerico = minutos * 60000LL + segundos * 1000LL + milisegundos;
	return tiempoNumerico;
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
	this->window_height = window_height;
	this->window_width = window_width;

	ui = new UI(window_width, window_height);
	
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

	parseCar("data/car2.scene", car ,shader);

	Vector3 car_pos = car->model.getTranslation();
	//--------------SECTORES
	finnish = new PrefabEntity("finnish", Vector3(223, 4, 612), shader, 1);
	finnish->model.scale(10, 2, 0.5);
	finnish->mesh = Mesh::Get("data/cube.obj");
	finnish->layer = FINISH;
	track->addChild(finnish);

	sector1 = new PrefabEntity("sector1", Vector3(-192, 4, 468), shader, 1);
	sector1->model.scale(10, 2, 0.5);
	sector1->mesh = Mesh::Get("data/cube.obj");
	sector1->layer = SECTOR1;
	track->addChild(sector1);

	sector2 = new PrefabEntity("sector2", Vector3(-132, 4, -702), shader, 1);
	sector2->model.scale(10, 3, 0.5);
	sector2->mesh = Mesh::Get("data/cube.obj");
	sector2->layer = SECTOR2;
	track->addChild(sector2);

	car->sectors[0] = false;
	car->sectors[1] = false;
	car->track_limits = true;

	setupTrackLimits();

	//Mejor Time
	std::ifstream archivo("data/tiempo.txt");
	std::string tiempo;

	if (!archivo) {
		std::cerr << "Error al abrir el archivo" << std::endl;
		tiempo = "59:59.999";
		best_time = convertirATiempoNumerico(tiempo);
		std::cout << best_time << std::endl;
	}
	else {
		while (std::getline(archivo, tiempo)) {
			tiempos.push_back(tiempo);
		}
		best_time = convertirATiempoNumerico(tiempos[0]);

		archivo.close();
	}

	//create our first person camera
	//camera->lookAt(Vector3(car_pos.x, car_pos.y + 3.6, car_pos.z - 3), Vector3(car_pos.x, car_pos.y + 3.39, car_pos.z + 1), Vector3(0.f, 1.f, 0.f));
	//camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	//create our third person camera	
	camera->lookAt(Vector3(car_pos.x, car_pos.y + 3, car_pos.z - 4), Vector3(car_pos.x, car_pos.y + 3, car_pos.z + 1), Vector3(0.f, 1.f, 0.f));
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	// Play channel
	//Audio::Play("data/sounds/DejaVu.wav");
	//Audio::Play3D("data/sounds/F1_sonido.wav", Vector3(200, 0, 500));
	Audio::Play("data/sounds/caster.wav");

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

std::string formatSpeed(long long speed) {
	// Construct the formatted time string
	std::stringstream ss;

	ss << std::setw(3) << std::setfill('0') << speed;

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
	//Draw GUI
	ui->drawMinimap(car, track);
	ui->drawGUI();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//drawText(370, 550, car->getGear(), Vector3(1, 1, 1), 2);
	//drawText(370, 570, std::to_string(car->getRotationSpeed()), Vector3(1, 1, 1), 2);

	//TODO Los drawText son una basura
	drawText(87, window_height - 125, formatSpeed((int)car->getSpeed()*5), Vector3(1, 1, 1), 3);
	drawText(window_width - 210, 20, "Time: " + formatTime(t.getTime()), Vector3(1, 1, 1), 2);
	if (car->track_limits) drawText(window_width - 30, 20, "TL", Vector3(1, 1, 1), 2);
	if(best_time < 3599998) drawText(window_width - 210, 45, "Best time: " + formatTime(best_time), Vector3(1, 1, 1), 2);
	else drawText(window_width - 210, 45, "Best time: ", Vector3(1, 1, 1), 2);
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
	Vector3 prevPos = car->model.getTranslation();
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
	car->move(dir, turn, dt, camera);
	std::vector<sCollisionData> collisions = std::vector<sCollisionData>();
	checkCarCollisions(collisions);
	Vector3 current_car_pos = car->getPosition();
	if (!collisions.empty()) {
		for each (sCollisionData collision in collisions)
		{
			if(collision.type == FLOOR)
				car->model.setPosition(current_car_pos.x, collisions[0].colPoint.y+1, current_car_pos.z);
			if (collision.type == WALL)
				car->model.setPosition(prevPos.x, prevPos.y, prevPos.z);
		}
	}
	//std::cout << current_car_pos.x << " " << current_car_pos.y << " " << current_car_pos.z << std::endl;
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

void PlayScene::guardarTiempos() {
	std::ofstream archivoSalida("data/tiempo.txt");

	if (!archivoSalida) {
		std::cerr << "Error al abrir el archivo de salida" << std::endl;
		return;
	}

	for (const auto& tiempo : tiempos) {
		archivoSalida << tiempo << std::endl;
	}

	archivoSalida.close();
}

bool comparadorTiempos(const std::string& tiempo1, const std::string& tiempo2) {
	long long tiempoNumerico1 = convertirATiempoNumerico(tiempo1);
	long long tiempoNumerico2 = convertirATiempoNumerico(tiempo2);

	return tiempoNumerico1 < tiempoNumerico2;
}

bool PlayScene::checkCarCollisions(std::vector<sCollisionData>& collisions) {
	float sphereRadius = 3.f;
	Vector3 colPoint, colNormal;
	float colisionDisatance = 1.0f;
	// For each collider entity “e” in root:
	for each (PrefabEntity* e in track->children)
	{
		Mesh* mesh = e->mesh;
		double distance = abs(car->getPosition().distance(e->model.getTranslation()));
		//don't test ray if mesh is far
		Vector3 groundCheckPos = car->getPosition() + Vector3(1, 0, 0);

		if (mesh->testRayCollision(e->model, car->getPosition() , Vector3(0, -1, 0), colPoint, colNormal, 50.f, false)) {
			if (e->layer & TRACK) {
				collisions.push_back({ colPoint,colNormal.normalize(), FLOOR });
			}
		}
		
		if (mesh->testSphereCollision(e->model, car->getPosition(), 18.0f, colPoint, colNormal)) {

			colNormal.normalize();

			if (colNormal.dot(Vector3(0, 1, 0)) > 0.8) {
				continue;
			}

			if (e->layer == FINISH) {
				t.stop();
				//Si se completa la vuelta correctamente
				if (car->sectors[0] == true && car->sectors[1] == true && car->track_limits == false) {
					std::string stiempo = formatTime(t.getTime());
					long long tiempo = convertirATiempoNumerico(stiempo);
					tiempos.push_back(stiempo);
					std::sort(tiempos.begin(), tiempos.end(), comparadorTiempos);
					if (best_time > tiempo) best_time = tiempo;
					guardarTiempos();
				}

				car->sectors[0] = false;
				car->sectors[1] = false;
				car->track_limits = false;
				t.reset();
				t.start();
			}
			else if (e->layer == SECTOR1) {
				if (car->sectors[0] == false) car->sectors[0] = true;
			}
			else if (e->layer == SECTOR2) {
				if (car->sectors[1] == false && car->sectors[0] == true) car->sectors[1] = true;
			}
			else if (e->layer == TRACK_LIMITS) {
				car->track_limits = true;
			}
			else {
				collisions.push_back({ colPoint, colNormal, WALL });
				/*
				Vector3& velocity = player->velocity;
				Vector3 newDir = velocity.dot(collisionNormal);
				newDir *= collisionNormal;

				velocity.x -= newDir.x;
				velocity.z -= newDir.z;
				*/
			}
		}		
	}
	// End loop

	return !collisions.empty();

}

void PlayScene::setupTrackLimits() {
	Vector3 positions[12] = {
		Vector3(259, 4, 848),
		Vector3(253, 4, 923),
		Vector3(-38, 4, 726),
		Vector3(-213, 4, 334),
		Vector3(-277, 4, 297),
		Vector3(-240, 4, -332),
		Vector3(-232, 4, -379),
		Vector3(-147, 4, -934),
		Vector3(-152, 4, -987),
		Vector3(-100, 4, -845),
		Vector3(150, 4, 175),
		Vector3(164, 4, 216)
	};

	Vector3 scales[12] = {
		Vector3(25, 4, 20),
		Vector3(20, 4, 9),
		Vector3(5, 4, 1),
		Vector3(8, 4, 1),
		Vector3(3, 4, 8),
		Vector3(20, 4, 8), //chicane antes de lo de kubica
		Vector3(15, 4, 10),
		Vector3(2, 4, 23), //horquilla
		Vector3(20, 4, 14),
		Vector3(3, 4, 3),
		Vector3(7, 4, 7),
		Vector3(12, 4, 11),
	};			

	for(int i = 0; i < 12; i++) {
		PrefabEntity* trackLimit = new PrefabEntity("trackLimit", positions[i], shader, 1);
		//trackLimit->model.scale(scales[i]);
		trackLimit->mesh = Mesh::Get("data/cube.obj");
		trackLimit->layer = TRACK_LIMITS;
		track->addChild(trackLimit);
	}
}