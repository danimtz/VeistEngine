#pragma once
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/Buffers/UniformBuffer.h"
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"

#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

//Not going to abstract vulkan for now from this class
//C++ rule of 3 maybe. thought shouldnt be needed since Renderer/ForwardRenderer etc class is only used with unique ptr in RenderModule
class ForwardRenderer : public Renderer
{
public:

	ForwardRenderer() = default;
	~ForwardRenderer();

	virtual void init(std::shared_ptr<RenderBackend> backend) override;
	virtual void renderScene(Scene *scene) override;

private:



	//Maybe put all this in a RendererData struct or something
	

	struct CameraData {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 view_projection;
	};

	struct GPUDirLight {
		glm::vec3 direction;
		float intensity;
		glm::vec3 colour;
		uint32_t num_lights;
	};

	CameraData m_camera_data;

	
	std::unique_ptr<UniformBuffer> m_camera_buffer; //Camera, environment etc (Updated per frame)
	std::unique_ptr<UniformBuffer> m_dir_lights_buffer; //Camera, environment etc (Updated per frame)
	std::vector<DescriptorSet> m_global_descriptor;



};

