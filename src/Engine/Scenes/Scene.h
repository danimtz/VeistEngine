#pragma once

#define GLM_DEPTH_ZERO_TO_ONE
#include "Engine/Scenes/Model.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Scenes/DirectionalLight.h"
#include "Engine/Scenes/PointLight.h"

//Temporary scene class. Must be reworked into a btter system later (Scenegraph, ECS etc etc)
class Scene
{
public: 

	Scene();


	const std::vector<Model>& getModels() const {return m_scene_models;}; // this function should order models by material then pipeline
	const std::vector<DirectionalLight>& getDirLights() const { return m_directional_lights; };
	const std::vector<PointLight>& getPointLights() const { return m_point_lights; };
	Camera* getCamera() const {return m_scene_camera.get();}


	void onUpdate();



private:

	std::unique_ptr<Camera> m_scene_camera;
	std::vector<Model> m_scene_models;
	std::vector<DirectionalLight> m_directional_lights;
	std::vector<PointLight> m_point_lights;

};

