#pragma once

#define GLM_DEPTH_ZERO_TO_ONE
#include "Engine/Scenes/Model.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Camera/CameraController.h"
#include "Engine/Scenes/DirectionalLight.h"
#include "Engine/Scenes/PointLight.h"
#include "Engine/Scenes/Skybox.h"
#include "Engine/Scenes/LightProbe.h"

#include "Engine/Scenes/ECS/EntityRegistry.h"


//Temporary scene class. Must be reworked into a btter system later (Scenegraph, ECS etc etc)
class Scene
{
public: 

	Scene();

	ecs::EntityRegistry* ecsRegistry() const { return m_registry.get();};

	void onUpdate(Timestep dt);



private:


	std::unique_ptr<ecs::EntityRegistry> m_registry;

	CameraController m_cam_control;


};

