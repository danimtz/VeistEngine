#pragma once


#include "RenderGraphPass.h"

#include <Veist/Graphics/RenderModule.h>


namespace Veist
{
namespace RenderGraph
{


	class PassBuilder
	{
	public:
	
		using RenderFunction = std::function<void(CommandBuffer&, const RenderGraphPass*)>;


		//Pass reads. Read resources must be declared respecting binding order inside the shader
		BufferResource* addUniformInput(const std::string &name, const BufferInfo& info,  PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		BufferResource* addUniformInput(const std::string& name, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);

		
		BufferResource* addStorageInput(const std::string& name, const BufferInfo& info,  PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		BufferResource* addStorageInput(const std::string& name, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);


		ImageResource* addTextureInput(const std::string& name, const ImageInfo& info, SamplerType sampler_type = SamplerType::RepeatLinear, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		ImageResource* addTextureInput(const std::string& name, SamplerType sampler_type = SamplerType::RepeatLinear, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		
		ImageResource* addDepthInput(const std::string& name, const ImageInfo& info, SamplerType sampler_type = SamplerType::RepeatLinear, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);
		ImageResource* addDepthInput(const std::string& name, SamplerType sampler_type = SamplerType::RepeatLinear, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);

		//ImageResource* addAttachmentInput(const std::string& name, const ImageInfo& info, SamplerType sampler_type, PipelineStage stage = PipelineStage::AnyShader, uint32_t d_set_index = 0);

		void addExternalInput(const std::string& name, Descriptor descriptor, const uint32_t d_set_index = 0); //Adds input from a resource external to the rendergraph environement.
		//todo add more


		//Pass writes:

		ImageResource* addColorOutput(const std::string& name, const ImageInfo& info = {}, const std::string& input = "");
		ImageResource* addDepthOutput(const std::string& name, const ImageInfo& info = {}, const std::string& input = "");

		//TODO "input" argument does nothing for these two functions
		ImageResource* addStorageTextureOutput(const std::string& name, const ImageInfo& info = {}, const std::string& input = "", PipelineStage stage = PipelineStage::ComputeShader);
		BufferResource* addStorageOutput(const std::string& name, const BufferInfo& info = {}, const std::string& input = "", PipelineStage stage = PipelineStage::ComputeShader);
		


		//ImageResource* addColorOutput(const std::string& name);
		//ImageResource* addDepthOutput(const std::string& name);
		//ImageResource* addStorageTextureOutput(const std::string& name, PipelineStage stage = PipelineStage::ComputeShader);
		//BufferResource* addStorageOutput(const std::string& name, PipelineStage stage = PipelineStage::ComputeShader);

		void setRenderFunction(RenderFunction&& function);


		//TODO remove this later when find a better way to transition output image for rendering to a texture in gui
		uint32_t getPassIndex() {return m_graph_pass->m_pass_index;};

	private:

		enum ResourceAction
		{
			Read,
			Write
		};

		ImageResource* addImageToPass(ResourceAction action, const std::string& name, const ImageInfo& info, PipelineStage stage, ImageUsage usage);
		BufferResource* addBufferToPass(ResourceAction action, const std::string& name, const BufferInfo& info, PipelineStage stage, ShaderBufferUsage usage);

		friend class RenderGraph;


		PassBuilder(RenderGraphPass* pass) : m_graph_pass(pass) {};
	
	

		RenderGraphPass* m_graph_pass{nullptr};

		
	};


}
}