#include "scene.h"
#include "utils.h"


PlayScene::PlayScene() {
	prefab_entities = std::vector<PrefabEntity*>();
	player = nullptr;
}

void PlayScene::setupScene(int window_width, int window_height) {

	player = new PrefabEntity("car", Vector3(1, 3, 1), "data/car.obj", "data/Image_13.png", Vector4(1, 1, 1, 1));
	prefab_entities.push_back(player);
	track = new  PrefabEntity("track", Vector3(1, 1, 1), "data/track.obj", "data/grass.png", Vector4(1, 1, 1, 1));
	prefab_entities.push_back(track);

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
}

void PlayScene::renderScene(Camera* camera) {
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (!shader)
		return;

	for (PrefabEntity* entity : prefab_entities) {
		//enable shader
		shader->enable();
		
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

void PlayScene::update(float dt) {

}