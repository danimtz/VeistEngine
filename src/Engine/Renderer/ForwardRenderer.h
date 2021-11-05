#pragma once

#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/Buffers/ShaderBuffer.h"
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"
#include "Engine/Renderer/Vulkan/Commands/CommandBuffer.h"

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
	virtual void setScene(Scene* scene) override;
	virtual void onUpdate() override;
	
	void renderScene(CommandBuffer& cmd_buffer);

private:


	
	Scene* m_scene {nullptr};
	
	//Maybe put all this in a RendererData struct or something
	static constexpr int MAX_DIR_LIGHTS = 4;
	static constexpr int MAX_POINT_LIGHTS = 100;

	struct CameraData {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 view_projection;
		glm::mat4 inverse_view;
	};

	struct GPUDirLight {
		glm::vec3 direction;
		float intensity;
		glm::vec3 colour;
		uint32_t padding;
	};

	struct GPUPointLight {
		glm::vec3 position;
		float intensity;
		glm::vec3 colour;
		float radius;
	};

	struct SceneInfo {
		float dir_lights_count;
		float point_lights_count;
		//more to be added
	};

	CameraData m_camera_data;
	SceneInfo m_scene_info;
	
	std::unique_ptr<ShaderBuffer> m_camera_buffer; //Camera, environment etc (Updated per frame)
	std::unique_ptr<ShaderBuffer> m_scene_info_buffer; //Camera, environment etc (Updated per frame)
	std::unique_ptr<ShaderBuffer> m_dir_lights_buffer; //Camera, environment etc (Updated per frame)
	std::unique_ptr<ShaderBuffer> m_point_lights_buffer;
	std::vector<DescriptorSet> m_global_descriptor;



};

