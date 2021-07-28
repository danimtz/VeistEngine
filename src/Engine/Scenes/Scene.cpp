#include "Scene.h"


Scene::Scene() {


	
	m_scene_camera = std::make_unique<Camera>();

	m_scene_camera->setPosition(glm::vec3{ 0.0f, 0.0f, -3.0f });


	m_scene_models.push_back({ "..\\..\\assets\\DamagedHelmet\\DamagedHelmet.gltf",  "helmet",  "..\\..\\src\\Shaders\\" }); //{ "..\\..\\assets\\Box\\Box With Spaces.gltf" }; //




	//I dont like how im using pipelines here. Pipelines are being used as a shader (which is kind of how it works in vulkan)
	//A better way i reckon would be to use shader or material here in the renderer and abstract the usage of the pipeline object away from whoever 
	//is writing the renderer class (which is me in this case but yeah). Then the shader class or material should contain stuff like Pipeline Layout and DescriptorSetLayout
	//and push constant range, VertexDescription(my own thing). Leaving it for now
	//m_test_pipeline = GraphicsPipeline::Create("helmet", "..\\..\\src\\Shaders\\", m_scene_meshes[0].getVertexBuffer()->getInputDescription()); //


}


void Scene::onUpdate() 
{

	m_scene_camera->onUpdate();


}