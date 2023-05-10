#include "entity.h"
#include "camera.h"
#include "shader.h"

Entity::Entity() {
}

Matrix44 Entity::getGlobalMatrix() {
	return Matrix44();
}


PrefabEntity::PrefabEntity(Vector3 position, const char* meshf, const char* texturef, const char* vsf, const char* fsf, Vector4 color) {
	mesh = Mesh::Get(meshf);
	if (texturef) texture = Texture::Get(texturef); else texture = new Texture();
	model.setTranslation(position.x, position.y, position.z);

	shader = new Shader();
	shader = Shader::Get(vsf, fsf);
	this->color = color;
}

void PrefabEntity::render() {
	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewproj", camera->viewprojection_matrix);
	shader->setTexture("u_texture", texture,0);


	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	shader->disable();
}