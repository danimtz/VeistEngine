#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() {


	
	m_scene_camera = std::make_unique<Camera>();

	m_scene_camera->setPosition(glm::vec3{ 0.0f, 0.0f, -3.0f });

	//m_scene_models.push_back({ "..\\..\\assets\\DamagedHelmet\\DamagedHelmet.gltf",  "descriptortest",  "..\\..\\src\\Shaders\\" }); //{ "..\\..\\assets\\Box\\Box With Spaces.gltf" }; //
	m_scene_models.push_back({ "..\\..\\assets\\DamagedHelmet\\DamagedHelmet.gltf",  "mainShader" }); //{ "..\\..\\assets\\Box\\Box With Spaces.gltf" }; //
	m_scene_models.push_back({ "..\\..\\assets\\DamagedHelmet\\DamagedHelmet.gltf",  "mainShader" }); //{ "..\\..\\assets\\Box\\Box With Spaces.gltf" }; //

	m_scene_models[0].modelMatrix() = glm::translate(m_scene_models[0].modelMatrix(), glm::vec3{1.0, 0.0, 0.0});
	m_scene_models[1].modelMatrix() = glm::translate(m_scene_models[1].modelMatrix(), glm::vec3{-1.0, 0.0, 0.0 });
	//m_scene_models[1].modelMatrix() = glm::scale(m_scene_models[1].modelMatrix(), glm::vec3{ 0.9, 0.5, 0.4 });




	//Lights

	glm::vec3 dir = glm::normalize(glm::vec3(1.0, 0.7, -1.0));
	glm::vec3 sky_dir = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 col = glm::vec3(1.0, 0.83, 0.51);
	m_directional_lights.push_back(DirectionalLight(sky_dir, glm::vec3(1.0), 0.5));//sun

	//m_directional_lights.push_back(DirectionalLight());
	//m_directional_lights.push_back(DirectionalLight(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.5, 0.8, 0.6)));//sky

	glm::vec3 pos = glm::vec3(-3.0, 0.0, 0.0);
	col = glm::vec3(0.7, 0.13, 0.21);
	m_point_lights.push_back(PointLight(pos, col)); 

	pos = glm::vec3(3.0, 0.0, 0.0);
	col = glm::vec3(0.1, 0.13, 0.81);
	m_point_lights.push_back(PointLight(pos, col));



	//I dont like how im using pipelines here. Pipelines are being used as a shader (which is kind of how it works in vulkan)
	//A better way i reckon would be to use shader or material here in the renderer and abstract the usage of the pipeline object away from whoever 
	//is writing the renderer class (which is me in this case but yeah). Then the shader class or material should contain stuff like Pipeline Layout and DescriptorSetLayout
	//and push constant range, VertexDescription(my own thing). Leaving it for now
	//m_test_pipeline = GraphicsPipeline::Create("helmet", "..\\..\\src\\Shaders\\", m_scene_meshes[0].getVertexBuffer()->getInputDescription()); //


}


void Scene::onUpdate() 
{

	m_scene_camera->onUpdate();


	//update lights, models etc


}