#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"

#include <cmath>
#include "audio.h"

#include <GL/glut.h>
#include <thread>

//some globals
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
Animation* anim = NULL;
FBO* fbo = NULL;
Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;
	gamepadisPressed = false;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//load one texture without using the Texture Manager (Texture::Get would use the manager)
	texture = new Texture();
 	//texture->load("data/texture.tga");
	camera = new Camera();
	intro_scene = new IntroScene(camera);
	intro_scene->setupScene(window_width, window_height);

	play_scene = new PlayScene(camera);
	play_scene->setupScene(window_width, window_height);
	current_scene = intro_scene;

	game_over = new GameOverScene(camera);
	game_over->setupScene(window_width, window_height);

	minimapCamera = new Camera();
	//minimapCamera.setPerspective(0, window_width, 0, window_height, 0.1f, 10000.f);
	minimapCamera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective


	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	current_scene->renderScene();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	current_scene->update(elapsed_time);
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
	}
}
//TODO maybe add this to scene?
void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
	if (event.button == SDL_BUTTON_LEFT && current_scene == intro_scene) {
		if (event.x >= 0 && event.x <= window_width && event.y >= 0 && event.y <= window_height) {
			current_scene = play_scene;
			channel = Audio::Play("data/sounds/caster.wav");

			play_scene->t.start();
		}
	}
	if (event.button == SDL_BUTTON_LEFT && current_scene == game_over) {
		if (event.x >= 0 && event.x <= window_width && event.y >= 0 && event.y <= window_height) {
			camera = new Camera();
			play_scene = new PlayScene(Game::instance->camera);
			play_scene->setupScene(window_width, window_height);
			current_scene = intro_scene;
			game_over->stopAudio();
		}
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	//mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
	play_scene->window_height = height;
	play_scene->window_width = width;
}