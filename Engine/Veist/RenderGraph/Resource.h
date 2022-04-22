#pragma once


#include <Veist/Graphics/RenderModule.h>
#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Buffers/ShaderBuffer.h"

namespace Veist
{
namespace RenderGraph
{

	struct BufferInfo
	{
		uint32_t size{0};
		uint32_t subbuffer_count{1};
	};

	struct ImageInfo
	{
		ImageProperties properties;
		ImageViewType view_type{ImageViewType::Flat};
	};


	enum class ResourceType
	{
		Buffer,
		Image
	};


	class PhysicalResource;
	
	class Resource
	{
	public:
		
		enum { Unused = ~0u };

		Resource(uint32_t index, ResourceType type) : m_type(type), m_index(index), m_read_in_pass_count(0), m_used_in_graph(false) {};
		virtual ~Resource() = default;

		ResourceType resourceType() const {return m_type;}
		uint32_t index() const { return m_index; }
		const std::string& name() const { return m_name; }
		PhysicalResource* physicalResource() { return m_physical_resource;}

		bool usedInGraph() const { return m_used_in_graph; }
		const std::unordered_set<uint32_t>& writtenInPasses() const { return m_written_in_passes; }
		const std::unordered_set<uint32_t>& readInPasses() const { return m_read_in_passes; }
		uint32_t& passReadsRefCount() {return m_read_in_pass_count;};

		void setUsedInGraph(bool is_used) {m_used_in_graph = is_used;}
		void setResourceName(const std::string& name) {m_name = name;}
		void setWrittenInPass(uint32_t pass_index) {m_written_in_passes.emplace(pass_index);}
		void setReadInPass(uint32_t pass_index);
		void addStage(PipelineStage stage, uint32_t pass_index);


	protected:
		friend class PhysicalResource;
		ResourceType m_type;

		PhysicalResource* m_physical_resource = nullptr;

		uint32_t m_index;										
		uint32_t m_read_in_pass_count; //refcount of pass reads 
		std::unordered_set<uint32_t> m_written_in_passes;		
		std::unordered_set<uint32_t> m_read_in_passes;			

		std::string m_name;

		std::unordered_map<uint32_t, PipelineStage> m_stage_in_pass;
	
		bool m_used_in_graph;
	
	};



	class ImageResource : public Resource
	{
	public:

		ImageResource(uint32_t index) : Resource(index, ResourceType::Image), m_usage(ImageUsage::None) {}


		/*void aliasImageResource(ImageResource* other)
		{
			//Merge usage in passes
			m_usage = m_usage | other->m_usage;

			//Merge image usage
			for (auto it : other->m_usage_in_pass)
			{
				addImageUsage(it.second, it.first);
			}

			//other->m_index = m_index;

			//disable old resource
			other->m_used_in_graph = false;

			//TODO assert that ImageInfo is identical for both, else it cant alias
		}*/


		void setImageInfo(const ImageInfo& info);
		void addImageUsage(ImageUsage usage, uint32_t pass_index);
		

		//ImageUsage imageUsage() const {return m_usage;};
		//ImageProperties imageProperties() const {return m_info.properties;};
		//ImageViewType imageViewType() const {return m_info.view_type;};



		//TODO move to physical resource
		/*ImageUsage imageUsageInPass(uint32_t pass_index) const 
		{
			auto it = m_usage_in_pass.find(pass_index);
			if (it == m_usage_in_pass.end())
			{
				CRITICAL_ERROR_LOG("Resource not used in pass index queried");
			}
			else
			{
				return it->second;
			}
		};*/


	private:
		friend class PhysicalImage;
		ImageInfo m_info;
		ImageUsage m_usage;
		std::unordered_map<uint32_t, ImageUsage> m_usage_in_pass;
	};


	class BufferResource : public Resource
	{
	public:

		BufferResource(uint32_t index) : Resource(index, ResourceType::Buffer) {}

		void setBufferInfo(const BufferInfo& info) { m_info = info; }
		void setBufferUsage(ShaderBufferUsage usage) { m_usage = usage; }

		//uint32_t bufferSize() const {return m_info.size;}; //Returns size of subbuffer
		//uint32_t subBufferCount() const { return m_info.subbuffer_count;};
		//ShaderBufferUsage bufferUsage() const {return m_usage;};
	private:
		friend class PhysicalBuffer;
		BufferInfo m_info;
		ShaderBufferUsage m_usage;

	};

	

	

	class PhysicalResource
	{
	public:
		enum { Unused = ~0u };

		virtual ~PhysicalResource() = default;

		ResourceType resourceType() const { return m_type; }
		uint32_t physicalIndex() const { return m_physical_index; }
		uint32_t lastUsedPass() const { return m_last_pass_used; }
		const std::unordered_set<uint32_t>& usedInPasses() const {return m_used_in_passes;};
		bool isUnused() const {return m_resource_aliases.empty();}

		void setPhysicalIndex(uint32_t index) { m_physical_index = index; }
		void setLastUsedPass(uint32_t index) { m_last_pass_used = index; }

		PipelineStage getStageInPass(uint32_t pass_index) const;

	protected:

		PhysicalResource(ResourceType type) : m_type(type) {};

		void aliasResource(Resource* resource);

		ResourceType m_type;

		uint32_t m_last_pass_used = Unused;
		uint32_t m_physical_index = Unused;

		std::unordered_map<uint32_t, PipelineStage> m_stage_in_pass;
		std::unordered_set<Resource*> m_resource_aliases;
		std::unordered_set<uint32_t> m_used_in_passes;
	};




	class PhysicalImage : public PhysicalResource
	{
	public:

		PhysicalImage(ImageResource* resource);

		void aliasImageResource(ImageResource* resource);
		ImageUsage imageUsage() const { return m_usage; };
		ImageProperties imageProperties() const { return m_info.properties; };
		ImageViewType imageViewType() const { return m_info.view_type; };
		ImageUsage imageUsageInPass(uint32_t pass_index) const;

	private:
		ImageInfo m_info;
		ImageUsage m_usage;
		std::unordered_map<uint32_t, ImageUsage> m_usage_in_pass;

	};




	class PhysicalBuffer : public PhysicalResource
	{
	public:

		PhysicalBuffer(BufferResource* resource);

		void aliasBufferResource(BufferResource* resource);
		uint32_t bufferSize() const { return m_info.size; }; //Returns size of subbuffer
		uint32_t subBufferCount() const { return m_info.subbuffer_count; };
		ShaderBufferUsage bufferUsage() const { return m_usage; };

	private:
		BufferInfo m_info;
		ShaderBufferUsage m_usage;
	};



}
}