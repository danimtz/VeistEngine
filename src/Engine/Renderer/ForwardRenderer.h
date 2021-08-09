#pragma once
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/Buffers/UniformBuffer.h"


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


	struct CameraData {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 view_projection;
	};

	CameraData m_camera_data;

	//Uniform buffers here im thinking. or in a struct, but in this calss for now
	std::shared_ptr<UniformBuffer> m_global_uniform; //Camera, environment etc (Updated per frame)

	//std::shared_ptr<DescriptorHandler> m_global_descriptor;

};

