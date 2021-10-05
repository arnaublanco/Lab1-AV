#ifndef MATERIAL_H
#define MATERIAL_H

#include "framework.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "extra/hdre.h"

class Material {
public:

	Shader* shader = NULL;
	Texture* texture = NULL;
	vec4 color;

	virtual void setUniforms(Camera* camera, Matrix44 model) = 0;
	virtual void render(Mesh* mesh, Matrix44 model, Camera * camera) = 0;
	virtual void renderInMenu() = 0;
};

class StandardMaterial : public Material {
public:

	StandardMaterial(); 
	~StandardMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};
 
//afegit per nos
class PhongMaterial : public StandardMaterial {
public: 
	Vector3 ambientMaterial;
	Vector3 diffuseMaterial;
	Vector3 specularMaterial;
	float alpha;

	PhongMaterial(Vector3 ambientMaterial, Vector3 diffuseMaterial, Vector3 specularMaterial, float alpha);
	void setUniforms(Camera* camera, Matrix44 model);
};

class WireframeMaterial : public StandardMaterial {
public:

	WireframeMaterial();
	~WireframeMaterial();

	void render(Mesh* mesh, Matrix44 model, Camera * camera);
};

/*
class MirrorMaterial : public StandardMaterial {
public: 
	MirrorMaterial();
	~MirrorMaterial();

	void setUniforms(Camera* camera, Matrix44 model);

};
*/
/*
class SkyBox : public StandardMaterial {
	SkyBox();
	~SkyBox();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
	void renderInMenu();
};
*/
#endif