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
		void addUniformInput(const std::string &name, const RenderGraphBufferInfo& info);
		void addStorageInput(const std::string& name, const RenderGraphBufferInfo& info);
		void addAttachmentInput(const std::string& name, const RenderGraphImageInfo& info);
		void addTextureInput(const std::string& name, const RenderGraphImageInfo& info);
		void addDepthAttachmentInput(const std::string& name, const RenderGraphImageInfo& info);
		//todo add more


		//Pass writes:

		void addColorOutput(const std::string& name, const RenderGraphImageInfo& info);
		void addDepthOutput(const std::string& name, const RenderGraphImageInfo& info);
		void addStorageTextureOutput(const std::string& name, const RenderGraphImageInfo& info);
		void addStorageOutput(const std::string& name, const RenderGraphBufferInfo& info);
		//todo add more


		void setRenderFunction(RenderFunction&& function);


	private:


		void declare_resource(const std::string& name);



		friend class RenderGraph;


		RenderGraphPassBuilder(RenderGraphPass* pass) : m_graph_pass(pass) {};
	
	

		RenderGraphPass* m_graph_pass{nullptr};

		
	};

}