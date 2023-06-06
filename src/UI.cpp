#include "UI.h"

UI::UI(int window_width, int window_height) {
	this->window_height = window_height;
	this->window_width = window_width;
	camera2D.view_matrix = Matrix44(); // Set View to identity
	camera2D.setOrthographic(0, window_width, 0, window_height, -1, 1);
	velocimeter = Texture::Get("data/velocimeter.png");
}

void UI::drawVelocimeter() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader = Shader::Get("data/shaders/GUI.vs", "data/shaders/GUI.fs");
	shader->enable();

	shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);

	Mesh quad;

	// Three vertices of the 1st triangle
	quad.vertices.push_back(Vector3(20, 200, 0));
	quad.uvs.push_back(Vector2(0, 1));
	quad.vertices.push_back(Vector3(20, 20, 0));
	quad.uvs.push_back(Vector2(0, 0));
	quad.vertices.push_back(Vector3(200, 20, 0));
	quad.uvs.push_back(Vector2(1, 0));

	// Three vertices of the 2nd triangle
	quad.vertices.push_back(Vector3(20, 200, 0));
	quad.uvs.push_back(Vector2(0, 1));
	quad.vertices.push_back(Vector3(200, 20, 0));
	quad.uvs.push_back(Vector2(1, 0));
	quad.vertices.push_back(Vector3(200, 200, 0));
	quad.uvs.push_back(Vector2(1, 1));

	// Send info to shader to render
	shader->setUniform("u_texture", velocimeter, 0);

	// Draw call
	quad.render(GL_TRIANGLES);

	shader->disable();
}

void UI::drawTime() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader = Shader::Get("data/shaders/GUI.vs", "data/shaders/flat.fs");
	shader->enable();

	shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);

	Mesh quad;

	// Three vertices of the 1st triangle
	quad.vertices.push_back(Vector3(window_width - 160, window_height, 0));
	quad.uvs.push_back(Vector2(0, 1));
	quad.vertices.push_back(Vector3(window_width - 160, window_height - 60, 0));
	quad.uvs.push_back(Vector2(0, 0));
	quad.vertices.push_back(Vector3(window_width, window_height - 60, 0));
	quad.uvs.push_back(Vector2(1, 0));

	// Three vertices of the 2nd triangle
	quad.vertices.push_back(Vector3(window_width - 160, window_height, 0));
	quad.uvs.push_back(Vector2(0, 1));
	quad.vertices.push_back(Vector3(window_width, window_height - 60, 0));
	quad.uvs.push_back(Vector2(1, 0));
	quad.vertices.push_back(Vector3(window_width, window_height, 0));
	quad.uvs.push_back(Vector2(1, 1));

	shader->setUniform("u_color", Vector4(0.0f, 0.0f, 0.0f, 0.9f));

	quad.render(GL_TRIANGLES);

	shader->disable();
}

void UI::drawMinimap(CarEntity* car, Entity* track) {
	glViewport(0, 0, 180, 180);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	Vector3 car_pos = car->model.getTranslation();
	Camera minimapCamera;
	minimapCamera.lookAt(Vector3(car_pos.x, car_pos.y + 100, car_pos.z), car_pos, Vector3(0.0f, 1.0f, 0.0f));
	minimapCamera.setOrthographic(0, window_width, 0, window_height, -1, 1);

	track->render(&minimapCamera);
	car->render(&minimapCamera);

	shader = Shader::Get("data/shaders/GUI.vs", "data/shaders/GUI.fs");
	shader->enable();

	shader->setUniform("u_viewprojection", minimapCamera.viewprojection_matrix);

	Mesh quad;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Three vertices of the 1st triangle
	quad.vertices.push_back(Vector3(400, 580, 0));
	quad.uvs.push_back(Vector2(0, 1));
	quad.vertices.push_back(Vector3(400, 400, 0));
	quad.uvs.push_back(Vector2(0, 0));
	quad.vertices.push_back(Vector3(580, 400, 0));
	quad.uvs.push_back(Vector2(1, 0));

	// Three vertices of the 2nd triangle
	quad.vertices.push_back(Vector3(400, 580, 0));
	quad.uvs.push_back(Vector2(0, 1));
	quad.vertices.push_back(Vector3(500, 400, 0));
	quad.uvs.push_back(Vector2(1, 0));
	quad.vertices.push_back(Vector3(580, 580, 0));
	quad.uvs.push_back(Vector2(1, 1));

	// Draw call
	quad.render(GL_TRIANGLE_FAN);

	glViewport(0, 0, window_width, window_height);

	shader->disable();
}

void UI::drawGUI() {
	drawVelocimeter();
	drawTime();
}