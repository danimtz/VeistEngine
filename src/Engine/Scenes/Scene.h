#pragma once

#define GLM_DEPTH_ZERO_TO_ONE
#include "Engine/Mesh/Mesh.h"
#include "Engine/Camera/Camera.h"

#include "Engine/Renderer/ShaderAndPipelines/GraphicsPipeline.h"//temp

//Temporary scene class. Must be reworked into a btter system later (Scenegraph, ECS etc etc)
class Scene
{
public:
	Scene();


	const std::vector<Mesh>& getMeshes() const {return m_scene_meshes;};
	Camera* getCamera() const {return m_scene_camera.get();}



	//VERY TEMPORARY:
	std::shared_ptr<GraphicsPipeline> m_test_pipeline;



private:

	std::unique_ptr<Camera> m_scene_camera;
	std::vector<Mesh> m_scene_meshes;

	

};

