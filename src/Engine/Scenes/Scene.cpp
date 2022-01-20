#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>


#include "Engine/Scenes/ECS/Components/MeshComponent.h"

Scene::Scene() {

	//ECS tests
	m_registry = std::make_unique<ecs::EntityRegistry>();
	ecs::EntityId entity0 = m_registry->createEntity();
	
	ecs::EntityId entity1 = m_registry->createEntity();
	ecs::EntityId entity2 = m_registry->createEntity();
	ecs::EntityId entity3 = m_registry->createEntity();

	m_registry->emplaceComponent<MeshComponent>(entity1);
	m_registry->emplaceComponent<MeshComponent>(entity2);

	m_registry->emplaceComponent<TestComponent>(entity3, 3, 4);
	m_registry->removeComponent<MeshComponent>(entity2);

	m_registry->emplaceComponent<TestComponent>(entity1, 4, 5);
	//auto test4 = m_registry->emplaceComponent<TestComponent>(entity2);
	
	
	
	//End ECS tests





	m_scene_camera = std::make_unique<Camera>();

	m_scene_camera->setPosition(glm::vec3{ 0.0f, 0.0f, 3.5f });

	//m_scene_models.push_back({ "..\\..\\assets\\DamagedHelmet\\DamagedHelmet.gltf",  "descriptortest",  "..\\..\\src\\Shaders\\" }); //{ "..\\..\\assets\\Box\\Box With Spaces.gltf" }; //
	//m_scene_models.push_back({ "..\\..\\assets\\Bottle\\",  "normalOnly", "WaterBottle.gltf" });



	m_scene_models.push_back({ "..\\..\\assets\\GLTF_models\\Bottle\\",  "PBRforward", "WaterBottle.gltf" }); //{ "..\\..\\assets\\Box\\Box With Spaces.gltf" }; //
	
	m_scene_models.push_back({ "..\\..\\assets\\GLTF_models\\DamagedHelmet\\",  "PBRforward", "DamagedHelmet.gltf" }); //{ "..\\..\\assets\\Box\\Box With Spaces.gltf" }; //

	
	m_scene_models[0].modelMatrix() = glm::translate(m_scene_models[0].modelMatrix(), glm::vec3{ 2.0, 0.0, 0.0 });
	m_scene_models[0].modelMatrix() = glm::rotate(m_scene_models[0].modelMatrix(), glm::radians(0.0f), glm::vec3{ 1.0f, 0.0f, .0f });
	m_scene_models[0].modelMatrix() = glm::scale(m_scene_models[0].modelMatrix(), glm::vec3{ 8, 8, 8 });



	m_scene_models[1].modelMatrix() = glm::translate(m_scene_models[1].modelMatrix(), glm::vec3{-1.0, 0.0, 0.0 });
	m_scene_models[1].modelMatrix() = glm::rotate(m_scene_models[1].modelMatrix(), glm::radians(180.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });
	m_scene_models[1].modelMatrix() = glm::rotate(m_scene_models[1].modelMatrix(), glm::radians(90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f });
	m_scene_models[1].modelMatrix() = glm::scale(m_scene_models[1].modelMatrix(), glm::vec3{ 1, 1, 1 });
	


	//Lights
	
	glm::vec3 sun_dir = glm::normalize(glm::vec3(0.0, 1.0, -1.0));
	
	m_directional_lights.push_back(DirectionalLight(sun_dir, glm::vec3(1.0), 1.0));//sun

	glm::vec3 sky_dir = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 col = glm::vec3(1.0, 0.83, 0.51);
	//m_directional_lights.push_back(DirectionalLight());
	//m_directional_lights.push_back(DirectionalLight(sky_dir, glm::vec3(0.5, 0.8, 0.6)));//sky

	glm::vec3 pos = glm::vec3(-3.0, 1.0, 0.0);
	col = glm::vec3(0.7, 0.13, 0.21);
	//m_point_lights.push_back(PointLight(pos, col)); 

	pos = glm::vec3(3.0, 0.0, 0.0);
	col = glm::vec3(0.1, 0.13, 0.81);
	//m_point_lights.push_back(PointLight(pos, col));

	pos = glm::vec3(0.2, 0.4, -1.0);
	col = glm::vec3(1.0);
	//m_point_lights.push_back(PointLight(pos, col));



	//Skybox

	//m_skybox = Skybox::createFromEquirectMap("Skybox", "..\\..\\assets\\Skyboxes\\Equirect\\Ice_Lake\\Ice_Lake_Ref.hdr");
	//m_skybox = Skybox::createFromEquirectMap("Skybox", "..\\..\\assets\\Skyboxes\\Equirect\\Winter_Forest\\WinterForest_Ref.hdr");
	m_skybox = Skybox::createFromEquirectMap("Skybox", "..\\..\\assets\\Skyboxes\\Equirect\\Venice\\Venice.hdr");

	//IBL probe


	//std::shared_ptr<Cubemap> computed_cube = AssetLoader::loadCubemapFromEquirectMap("..\\..\\assets\\Skyboxes\\Equirect\\Ice_Lake\\Ice_Lake_Ref.hdr", true);
	//std::shared_ptr<Cubemap> computed_cube = AssetLoader::loadCubemapFromEquirectMap("..\\..\\assets\\Skyboxes\\Equirect\\Winter_Forest\\WinterForest_Ref.hdr", true);
	std::shared_ptr<Cubemap> computed_cube = AssetLoader::loadCubemapFromEquirectMap("..\\..\\assets\\Skyboxes\\Equirect\\Venice\\Venice.hdr", true);

	m_global_probe = std::make_unique<LightProbe>(*computed_cube.get());

}


void Scene::onUpdate() 
{

	m_cam_control.updateCamera(*m_scene_camera.get());

	m_scene_camera->onUpdate();


	//update lights, models etc


}