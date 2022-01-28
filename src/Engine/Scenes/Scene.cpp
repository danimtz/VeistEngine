#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>


#include "Engine/Scenes/ECS/Components/Components.h"

Scene::Scene() {

	m_registry = std::make_unique<ecs::EntityRegistry>();

	ecs::EntityId camera = m_registry->createEntity();
	ecs::EntityId waterbottle = m_registry->createEntity();
	ecs::EntityId helmet = m_registry->createEntity();
	ecs::EntityId damagedhelmet = m_registry->createEntity();

	ecs::EntityId point_light_1 = m_registry->createEntity();
	ecs::EntityId sun_light = m_registry->createEntity();
	ecs::EntityId skybox = m_registry->createEntity();


	m_registry->emplaceComponent<CameraComponent>(camera);
	m_registry->emplaceComponent<TransformComponent>(camera, glm::vec3{ 0.0f, 0.0f, 3.5f });


	glm::vec3 pos = glm::vec3(3.0, 0.0, 0.0);
	glm::vec3 col = glm::vec3(0.1, 0.13, 0.81);
	m_registry->emplaceComponent<PointLightComponent>(point_light_1, col);
	m_registry->emplaceComponent<TransformComponent>(point_light_1, pos);
	
	
	m_registry->emplaceComponent<DirectionalLightComponent>(sun_light, glm::normalize(glm::vec3(0.0, 1.0, -1.0)), glm::vec3(1.0), 1.0);

	m_registry->emplaceComponent<MeshComponent>(waterbottle, Model("..\\..\\assets\\GLTF_models\\Bottle\\",  "PBRforward", "WaterBottle.gltf"));
	m_registry->emplaceComponent<TransformComponent>(waterbottle, glm::vec3{ 2.0, 0.0, 0.0 }, glm::vec3{ 0.0, 0.0, 0.0}, glm::vec3{ 8.0, 8.0, 8.0 });
	
	m_registry->emplaceComponent<MeshComponent>(helmet, Model("..\\..\\assets\\GLTF_models\\DamagedHelmet\\", "PBRforward", "DamagedHelmet.gltf"));
	m_registry->emplaceComponent<TransformComponent>(helmet, glm::vec3{ -1.0, 0.0, 0.0 }, glm::vec3{ 0.0, 0.0, 0.0 }, glm::vec3{ 1.0, 1.0, 1.0 });


	m_registry->emplaceComponent<SkyboxComponent>(skybox, Skybox("Skybox", "..\\..\\assets\\Skyboxes\\Equirect\\Venice\\Venice.hdr", false));

	//IBL probe
	std::shared_ptr<Cubemap> computed_cube = AssetLoader::loadCubemapFromEquirectMap("..\\..\\assets\\Skyboxes\\Equirect\\Venice\\Venice.hdr", true);
	//TODO Creating LightProbes and LightProbe components needs to be waaaaaaay smoother and not include possible large copies of textures etc
	ecs::EntityId light_probe = m_registry->createEntity();
	m_registry->emplaceComponent<LightProbeComponent>(light_probe, LightProbe(*computed_cube.get()));
}


void Scene::onUpdate() 
{

	
	
	
	auto& scene_view = m_registry->view<CameraComponent>();
	for (ecs::EntityId entity : scene_view)
	{
		auto& cam_comp = scene_view.get<CameraComponent>(entity);
		auto& transform = scene_view.get<TransformComponent>(entity);


		m_cam_control.updateCamera(cam_comp.camera(), transform.translation()); 
		cam_comp.camera().onUpdate();


		break;//Only first camera componenet being taken into consideration for now
	}

	


	//Iterate ECS update transforms compoenents etc etc TODO
	
	


}