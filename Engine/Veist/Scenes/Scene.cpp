#include "pch.h"
#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Veist/Scenes/ECS/Components/Components.h"

namespace Veist
{

	Scene::Scene() {

		m_registry = std::make_unique<ecs::EntityRegistry>();

	
	}

	void Scene::loadScene(ecs::EntityRegistry* registry)
	{
		ecs::EntityId camera = registry->createEntity();
		ecs::EntityId waterbottle = registry->createEntity();

		ecs::EntityId damagedhelmet = registry->createEntity();
		ecs::EntityId helmet = registry->createEntity();

		ecs::EntityId point_light_1 = registry->createEntity();
		ecs::EntityId sun_light = registry->createEntity();
		ecs::EntityId skybox = registry->createEntity();


		registry->emplaceComponent<CameraComponent>(camera);
		registry->emplaceComponent<TransformComponent>(camera, glm::vec3{ 0.0f, 0.0f, 3.5f });


		glm::vec3 pos = glm::vec3(3.0, 0.0, 0.0);
		glm::vec3 col = glm::vec3(0.1, 0.13, 0.81);
		registry->emplaceComponent<PointLightComponent>(point_light_1, col);
		registry->emplaceComponent<TransformComponent>(point_light_1, pos);


		registry->emplaceComponent<DirectionalLightComponent>(sun_light, glm::normalize(glm::vec3(0.0, 1.0, -1.0)), glm::vec3(1.0), 1.0);

		registry->emplaceComponent<MeshComponent>(waterbottle, Model("..\\assets\\GLTF_models\\Bottle\\", "PBRforward", "WaterBottle.gltf"));
		registry->emplaceComponent<TransformComponent>(waterbottle, glm::vec3{ 2.0, 0.0, 0.0 }, glm::vec3{ 0.0, 0.0, 0.0 }, glm::vec3{ 8.0, 8.0, 8.0 });

		registry->emplaceComponent<MeshComponent>(helmet, Model("..\\assets\\GLTF_models\\DamagedHelmet\\", "PBRforward", "DamagedHelmet.gltf"));
		registry->emplaceComponent<TransformComponent>(helmet, glm::vec3{ -1.0, 0.0, 0.0 }, glm::vec3{ 90.0, 90.0, 0.0 }, glm::vec3{ 1.0, 1.0, 1.0 });


		registry->emplaceComponent<SkyboxComponent>(skybox, Skybox("Skybox", "..\\assets\\Skyboxes\\Equirect\\Venice\\Venice.hdr", false));

		//IBL probe
		std::shared_ptr<Cubemap> computed_cube = AssetLoader::loadCubemapFromEquirectMap("..\\assets\\Skyboxes\\Equirect\\Venice\\Venice.hdr", true);
		//TODO Creating LightProbes and LightProbe components needs to be waaaaaaay smoother and not include possible large copies of textures etc
		ecs::EntityId light_probe = registry->createEntity();
		registry->emplaceComponent<LightProbeComponent>(light_probe, LightProbe(*computed_cube.get()));

		registry->setSceneLoaded(true);
	}


	void Scene::onUpdate(Timestep dt) 
	{

	
	
		{
			auto& scene_view = m_registry->view<CameraComponent, TransformComponent>();
			for (ecs::EntityId entity : scene_view)
			{
				auto& cam_comp = scene_view.get<CameraComponent>(entity);
				auto& transform = scene_view.get<TransformComponent>(entity);


				m_cam_control.updateCamera(cam_comp.camera(), transform.translation(), dt); 
				cam_comp.camera().onUpdate();


				break;//Only first camera componenet being taken into consideration for now
			}
		}
	


		//Iterate ECS update transforms compoenents etc etc TODO
		{
		

			auto& scene_view = m_registry->view<MeshComponent, TransformComponent>();
			for (ecs::EntityId entity : scene_view)
			{
				auto& transform = scene_view.get<TransformComponent>(entity);

				transform.rotation().x += 5.0f * dt.getSeconds();
				break;
			}
		}
	

	}
}