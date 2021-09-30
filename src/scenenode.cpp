#include "scenenode.h"
#include "application.h"
#include "texture.h"
#include "utils.h"

unsigned int SceneNode::lastNameId = 0;
unsigned int mesh_selected = 0;

Light::Light(Vector3 position, Vector3 diffuseLight, Vector3 specularLight, Vector3 ambientLight) {
	this->position.set(position.x, position.y, position.z);
	this->diffuseLight.set(diffuseLight.x, diffuseLight.y, diffuseLight.z);
	this->specularLight.set(specularLight.x, specularLight.y, specularLight.z);
	this->ambientLight.set(ambientLight.x, ambientLight.y, ambientLight.z);
}

void Light::setUniforms(Shader* shader) {
	shader->setUniform("light_pos", position);
	shader->setUniform("ambientLight", ambientLight);
	shader->setUniform("diffuseLight", diffuseLight);
	shader->setUniform("specularLight", specularLight);
}

SceneNode::SceneNode()
{
	this->name = std::string("Node" + std::to_string(lastNameId++));
}


SceneNode::SceneNode(const char * name)
{
	this->name = name;
}

SceneNode::~SceneNode()
{

}

void SceneNode::render(Camera* camera)
{
	
	if (material)
		material->render(mesh, model, camera);
		material->shader->enable();
		light->setUniforms(material->shader);
		material->shader->disable();
}

void SceneNode::renderWireframe(Camera* camera)
{
	WireframeMaterial mat = WireframeMaterial();
	mat.render(mesh, model, camera);
}

void SceneNode::renderInMenu()
{
	//Model edit
	if (ImGui::TreeNode("Model")) 
	{
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(model.m, matrixTranslation, matrixRotation, matrixScale);
		ImGui::DragFloat3("Position", matrixTranslation, 0.1f);
		ImGui::DragFloat3("Rotation", matrixRotation, 0.1f);
		ImGui::DragFloat3("Scale", matrixScale, 0.1f);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, model.m);
		
		ImGui::TreePop();
	}

	//Material
	if (material && ImGui::TreeNode("Material"))
	{
		material->renderInMenu();
		ImGui::TreePop();
	}

	//Geometry
	if (mesh && ImGui::TreeNode("Geometry"))
	{
		bool changed = false;
		changed |= ImGui::Combo("Mesh", (int*)&mesh_selected, "SPHERE\0HELMET\0");

		ImGui::TreePop();
	}
}
