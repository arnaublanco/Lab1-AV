#include "application.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "volume.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include "extra/hdre.h"
#include "extra/imgui/imgui.h"
#include "extra/imgui/imgui_impl_sdl.h"
#include "extra/imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <string>

#include <cmath>

using namespace std;

bool render_wireframe = false;
Camera* Application::camera = nullptr;
Application* Application::instance = NULL;
//Shader* shader;


Application::Application(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;
	render_debug = true;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;
	scene_exposure = 1;
	output = 0;

	// OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	// Create camera
	camera = new Camera();
	camera->lookAt(Vector3(5.f, 5.f, 5.f), Vector3(0.f, 0.0f, 0.f), Vector3(0.f, 1.f, 0.f));
	camera->setPerspective(45.f,window_width/(float)window_height,0.1f,10000.f); //set the projection, we want to be perspective

	{

		SkyBoxNode* background = new SkyBoxNode("Background");
		StandardMaterial* matBG = new StandardMaterial();

		Texture* cubemap = new Texture();
		cubemap->cubemapFromImages("data/environments/snow");

		matBG->texture = cubemap;
		matBG->shader = Shader::Get("data/shaders/basicSkybox.vs", "data/shaders/skybox.fs");
		background->material = matBG;
		background->mesh = Mesh::Get("data/meshes/box.ASE.mbin");
		node_list.push_back(background);

		//Light
		Vector3 posLight = Vector3(60, 60, 0);
		Vector3 diffuseLight = Vector3(0.6f, 0.6f, 0.6f);
		Vector3 specularLight = Vector3(0.6f, 0.6f, 0.6f);
		Vector3 ambientLight = Vector3(0.6f, 0.6f, 0.6f);

		Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
		Shader* phong = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
		Shader* shaderMirror = Shader::Get("data/shaders/basic.vs", "data/shaders/mirror.fs");

		Light* light = new Light(posLight, diffuseLight, specularLight, ambientLight);
		light->name = "Light";
		StandardMaterial* lightMaterial = new StandardMaterial();
		lightMaterial->shader = shader;
		light->material = lightMaterial;
		node_list.push_back(light);

		Vector3 ambientMaterial = Vector3(0.6f, 0.6f, 0.6f);
		Vector3 diffuseMaterial = Vector3(0.6f, 0.6f, 0.6f);
		Vector3 specularMaterial = Vector3(0.6f, 0.6f, 0.6f);
		float alpha = 10;

		PhongMaterial* matMirror = new PhongMaterial(ambientMaterial, diffuseMaterial, specularMaterial, alpha); //ho canviem pel nostre
		matMirror->isMirror = 1;

		SceneNode* mirror = new SceneNode("Mirror ball");
		mirror->mesh = Mesh::Get("data/meshes/sphere.obj.mbin");
		mirror->material = matMirror;
		matMirror->shader = shaderMirror;
		mirror->model.setTranslation(-2, 0, 0);
		
		node_list.push_back(mirror);

		PhongMaterial* matTexture = new PhongMaterial(ambientMaterial, diffuseMaterial, specularMaterial, alpha); //ho canviem pel nostre
		matTexture->texture = Texture::Get("data/models/ball/metalness.png");

		SceneNode* nodeTexture = new SceneNode("Textured ball");
		nodeTexture->mesh = Mesh::Get("data/meshes/sphere.obj.mbin");
		matTexture->shader = shader;
		nodeTexture->material = matTexture;
		nodeTexture->model.setTranslation(2, 0, 0);

		node_list.push_back(nodeTexture);
		
	}

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Application::render(void)
{
	//set the clear color (the background color)
	glClearColor(.1,.1,.1, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//lo del type no va, no entre mai al primer if, si dona algun problema es que s'ha de posar shader enable abans
	for (size_t i = 0; i < node_list.size(); i++) {
		if (node_list[i]->name == "Light") {
			Light* light = (Light*)node_list[i]; // Downcast of Light
			light->material->shader->enable();
			light->setUniforms();
			light->material->shader->disable();
		}
		else {
			node_list[i]->material->shader->enable();
			node_list[i]->render(camera);
			node_list[i]->material->shader->disable();
		}

		if (render_wireframe) {
			node_list[i]->material->shader->enable();
			node_list[i]->renderWireframe(camera);
			node_list[i]->material->shader->disable();
		}
	}

	//Draw the floor grid
	if(render_debug)
		drawGrid();
}

void Application::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * 10; //the speed is defined by the seconds_elapsed so it goes constant
	float orbit_speed = seconds_elapsed * 1;
	
	//example
	float angle = seconds_elapsed * 10.f * DEG2RAD;
	/*for (int i = 0; i < root.size(); i++) {
		root[i]->model.rotate(angle, Vector3(0,1,0));
	}*/

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT && !ImGui::IsAnyWindowHovered() 
		&& !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())) //is left button pressed?
	{
		camera->orbit(-Input::mouse_delta.x * orbit_speed, Input::mouse_delta.y * orbit_speed);
	}

	//async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move fast er with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) camera->moveGlobal(Vector3(0.0f, -1.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_LCTRL)) camera->moveGlobal(Vector3(0.0f,  1.0f, 0.0f) * speed);
	
	for (size_t i = 0; i < node_list.size(); i++) {
		if (node_list[i]->name == "Background") {
			node_list[i]->model.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
		}
	}
	
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: render_debug = !render_debug; break;
		case SDLK_F2: render_wireframe = !render_wireframe; break;
		case SDLK_F5: Shader::ReloadAll(); break; 
	}
}

void Application::onKeyUp(SDL_KeyboardEvent event)
{
}

void Application::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Application::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Application::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Application::onMouseWheel(SDL_MouseWheelEvent event)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (event.type)
	{
		case SDL_MOUSEWHEEL:
		{
			if (event.x > 0) io.MouseWheelH += 1;
			if (event.x < 0) io.MouseWheelH -= 1;
			if (event.y > 0) io.MouseWheel += 1;
			if (event.y < 0) io.MouseWheel -= 1;
		}
	}

	if(!ImGui::IsAnyWindowHovered() && event.y)
		camera->changeDistance(event.y * 0.5);
}

void Application::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

void Application::onFileChanged(const char* filename)
{
	Shader::ReloadAll();
}
