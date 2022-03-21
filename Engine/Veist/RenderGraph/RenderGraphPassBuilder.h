#pragma once


#include "RenderGraphPass.h"

#include <Veist/Graphics/RenderModule.h>


namespace Veist
{

	class RenderGraphPassBuilder
	{
	public:
	
		using RenderFunction = std::function<void(CommandBuffer&, const RenderGraphPass*)>;


		//Pass reads

		//TODO: each of these functions needs to add name and rendergraphresource to RenderGraphPass->RenderGraph resource_to_index map and save the resource in the resource vector if it doenst exists yet
		RenderGraphBufferResource* addUniformInput(const std::string &name, const RenderGraphBufferInfo& info, const uint32_t d_set_index = 0);
		RenderGraphBufferResource* addStorageInput(const std::string& name, const RenderGraphBufferInfo& info, const uint32_t d_set_index = 0);
		RenderGraphImageResource* addAttachmentInput(const std::string& name, const RenderGraphImageInfo& info, const uint32_t d_set_index = 0);
		RenderGraphImageResource* addTextureInput(const std::string& name, const RenderGraphImageInfo& info, const uint32_t d_set_index = 0);
		RenderGraphImageResource* addDepthInput(const std::string& name, const RenderGraphImageInfo& info, const uint32_t d_set_index = 0);

		void addExternalInput(const std::string& name, Descriptor descriptor, const uint32_t d_set_index = 0); //Adds input from a resource external to the rendergraph environement.
		//todo add more


		//Pass writes:

		RenderGraphImageResource* addColorOutput(const std::string& name, const RenderGraphImageInfo& info);
		RenderGraphImageResource* addDepthOutput(const std::string& name, const RenderGraphImageInfo& info);
		RenderGraphImageResource* addStorageTextureOutput(const std::string& name, const RenderGraphImageInfo& info);
		RenderGraphBufferResource* addStorageOutput(const std::string& name, const RenderGraphBufferInfo& info);
		//todo add more


		void setRenderFunction(RenderFunction&& function);


	private:


		void declare_resource(const std::string& name);



		friend class RenderGraph;


		RenderGraphPassBuilder(RenderGraphPass* pass) : m_graph_pass(pass) {};
	
	

		RenderGraphPass* m_graph_pass{nullptr};

		
	};

}