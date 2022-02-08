#pragma once

#define GLM_DEPTH_ZERO_TO_ONE
#include "Veist/Scenes/Model.h"
#include "Veist/Scenes/Skybox.h"
#include "Veist/Camera/Camera.h"
#include "Veist/Scenes/LightProbe.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"


namespace Veist
{

//Temporary scene class. Must be reworked into a btter system later (Scenegraph, ECS etc etc)
class Scene
{
public: 

	Scene();

	//TEMPORARY
	void loadScene(ecs::EntityRegistry* registry);

	ecs::EntityRegistry* ecsRegistry() const { return m_registry.get();};

	void onUpdate(Timestep dt);

	Camera* getMainCamera() {return m_main_camera.get();};

private:


	std::unique_ptr<ecs::EntityRegistry> m_registry;
	std::shared_ptr<Camera> m_main_camera; //Shares with CameraComponent
	


};

}
