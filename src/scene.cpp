#include "scene.h"


PlayScene::PlayScene() {
	entities = std::vector<PrefabEntity*>();
	player = nullptr;
}

void PlayScene::setupScene() {
	player = new PrefabEntity(Vector3(1, 3, 1), "data/car.obj", "data/Image_13.png", "data/shaders/basic.vs", "data/shaders/texture.fs", Vector4(1,1,1,1));
}