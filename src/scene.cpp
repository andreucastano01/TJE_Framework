#include "scene.h"
#include "utils.h"
#include "extra\hdre.h"
#include "input.h"

PlayScene::PlayScene() {
	prefab_entities = std::vector<PrefabEntity*>();
	player = nullptr;
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

	player = new PrefabEntity("car", Vector3(1, 3, 1), "data/car.obj", "data/Image_13.png", Vector4(1, 1, 1, 1));
	prefab_entities.push_back(player);
	track = new PrefabEntity("track", Vector3(1, 1, 1), "data/track.obj", "data/grass.png", Vector4(1, 1, 1, 1));
	prefab_entities.push_back(track);
	player = track;

	skybox = CubemapFromHDRE("data/panorama.hdre");

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
}

void PlayScene::generateSkybox(Camera* camera) {
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

void PlayScene::renderScene(Camera* camera) {
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (!shader)
		return;

	generateSkybox(camera);

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

}

void PlayScene::update(float dt, Camera* camera) {
	float speed = dt * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)dt * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}
	

	Vector3 carPos = player->model.getTranslation();
	
	
	float angle = 0.0f;

	//async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) {
		carPos.z -= speed * 3;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
		carPos.z += speed * 3;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
		//camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		//camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	}

	
	player->model.setTranslation(carPos.x, carPos.y, carPos.z);
			//entity->model.rotate(angle, Vector3(0.0, 1.0, 0.0));
	
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}