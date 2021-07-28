#pragma once

#define GLM_DEPTH_ZERO_TO_ONE
#include "Model.h"
#include "Engine/Camera/Camera.h"

#include "Engine/Renderer/ShaderAndPipelines/GraphicsPipeline.h"//temp

//Temporary scene class. Must be reworked into a btter system later (Scenegraph, ECS etc etc)
class Scene
{
public:
	Scene();


	const std::vector<Model>& getModels() const {return m_scene_models;};
	Camera* getCamera() const {return m_scene_camera.get();}


	void onUpdate();



private:

	std::unique_ptr<Camera> m_scene_camera;
	std::vector<Model> m_scene_models;

	

};

