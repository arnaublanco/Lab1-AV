#include "material.h"
#include "texture.h"
#include "application.h"
#include "extra/hdre.h"

StandardMaterial::StandardMaterial(){ 
	color = vec4(1.f, 1.f, 1.f, 1.f);
	this->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

StandardMaterial::~StandardMaterial()
{

}

void StandardMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Application::instance->time);
	shader->setUniform("u_output", Application::instance->output);

	shader->setUniform("u_color", color);
	shader->setUniform("u_exposure", Application::instance->scene_exposure);

	shader->setVector3("ambientMaterial", ambientMaterial);
	shader->setVector3("diffuseMaterial", diffuseMaterial);
	shader->setVector3("specularMaterial", specularMaterial);
	shader->setFloat("alpha", alpha);

	if (texture)
		shader->setUniform("u_texture", texture);
}

void StandardMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void StandardMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
}

//Material es la clase base, StandarMat tambe es un tipus de clase base pero amb mes coses. Igual que el profe fa el Wireframe material 
// nosaltres fem el PhonMaterial. Si posem mes coses a standard despres tambe les hem de posar a Wire.
// creem el phong per 
PhongMaterial::PhongMaterial(Shader* shader){

	ambientMaterial = Vector3(0.6f, 0.6f, 0.6f);
	diffuseMaterial = Vector3(0.6f, 0.6f, 0.6f);
	specularMaterial = Vector3(0.6f, 0.6f, 0.6f);
	alpha = 10;

	if (shader) {
		this->shader = shader;
	} 
	else {
		this->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	}
}

WireframeMaterial::WireframeMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

WireframeMaterial::~WireframeMaterial()
{

}

void WireframeMaterial::render(Mesh* mesh, Matrix44 model, Camera * camera)
{
	if (shader && mesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//enable shader
		shader->enable();

		//upload material specific uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}