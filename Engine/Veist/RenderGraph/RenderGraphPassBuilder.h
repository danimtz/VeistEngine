#pragma once


#include "RenderGraphPass.h"

#include <Veist/Graphics/RenderModule.h>


namespace Veist
{

	class RenderGraphPassBuilder
	{
	public:
	
		using RenderFunction = std::function<void(CommandBuffer&, const RenderGraphPass*)>;


		//Pass reads. Read resources must be declared respecting binding order inside the shader
		RenderGraphBufferResource* addUniformInput(const std::string &name, const RenderGraphBufferInfo& info,  PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		RenderGraphBufferResource* addUniformInput(const std::string& name, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);

		
		RenderGraphBufferResource* addStorageInput(const std::string& name, const RenderGraphBufferInfo& info,  PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		RenderGraphBufferResource* addStorageInput(const std::string& name, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);

		//TODO add resource info-less versions of functions and change getOrAddResource to two seperate functions.

		RenderGraphImageResource* addTextureInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type = SamplerType::RepeatLinear, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		RenderGraphImageResource* addTextureInput(const std::string& name, SamplerType sampler_type = SamplerType::RepeatLinear, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		
		RenderGraphImageResource* addDepthInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type = SamplerType::RepeatLinear, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		RenderGraphImageResource* addDepthInput(const std::string& name, SamplerType sampler_type = SamplerType::RepeatLinear, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);

		//RenderGraphImageResource* addAttachmentInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);

		void addExternalInput(const std::string& name, Descriptor descriptor, const uint32_t d_set_index = 0); //Adds input from a resource external to the rendergraph environement.
		//todo add more


		//Pass writes:

		RenderGraphImageResource* addColorOutput(const std::string& name, const RenderGraphImageInfo& info);
		RenderGraphImageResource* addColorOutput(const std::string& name);

		RenderGraphImageResource* addDepthOutput(const std::string& name, const RenderGraphImageInfo& info);
		RenderGraphImageResource* addDepthOutput(const std::string& name);

		RenderGraphImageResource* addStorageTextureOutput(const std::string& name, const RenderGraphImageInfo& info, PipelineStage stage = PipelineStage::ComputeShader);
		RenderGraphImageResource* addStorageTextureOutput(const std::string& name, PipelineStage stage = PipelineStage::ComputeShader);

		RenderGraphBufferResource* addStorageOutput(const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage = PipelineStage::ComputeShader);
		RenderGraphBufferResource* addStorageOutput(const std::string& name, PipelineStage stage = PipelineStage::ComputeShader);
		//todo add more


		void setRenderFunction(RenderFunction&& function);


	private:

		enum ResourceAction
		{
			Read,
			Write
		};

		RenderGraphImageResource* addImageToPass(ResourceAction action, const std::string& name, const RenderGraphImageInfo& info, PipelineStage stage, ImageUsage usage);
		RenderGraphBufferResource* addBufferToPass(ResourceAction action, const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage, ShaderBufferUsage usage);

		friend class RenderGraph;


		RenderGraphPassBuilder(RenderGraphPass* pass) : m_graph_pass(pass) {};
	
	

		RenderGraphPass* m_graph_pass{nullptr};

		
	};

}