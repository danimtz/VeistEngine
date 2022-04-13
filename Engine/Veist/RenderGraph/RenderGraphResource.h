#pragma once


#include <Veist/Graphics/RenderModule.h>
#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Buffers/ShaderBuffer.h"

namespace Veist
{

	struct RenderGraphBufferInfo
	{
		uint32_t size{0};
		uint32_t subbuffer_count{1};
	};

	struct RenderGraphImageInfo
	{
		ImageProperties properties;
		ImageViewType view_type{ImageViewType::Flat};
	};


	
	class RenderGraphResource
	{
	public:
		enum { Unset = ~0u };

		enum class ResourceType
		{
			Buffer,
			Image
		};
		RenderGraphResource(uint32_t index, ResourceType type) : m_type(type), m_index(index), m_read_in_pass_count(0), m_used_in_graph(false) {};

		ResourceType resourceType() const {return m_type;}

		uint32_t index() const { return m_index; }

		uint32_t physicalIndex() const { return m_physical_index; }
		

		bool usedInGraph() const { return m_used_in_graph; }

		const std::unordered_set<uint32_t>& writtenInPasses() const { return m_written_in_passes; }

		const std::unordered_set<uint32_t>& readInPasses() const { return m_read_in_passes; }

		uint32_t& passReadsRefCount() {return m_read_in_pass_count;};


		void setUsedInGraph(bool is_used)
		{
			m_used_in_graph = is_used;
		}

		void setResourceName(const std::string& name)
		{
			m_name = name;
		}


		void setWrittenInPass(uint32_t pass_index)
		{
			m_written_in_passes.emplace(pass_index);
		}

		void setReadInPass(uint32_t pass_index)
		{
			m_read_in_passes.emplace(pass_index);
			m_read_in_pass_count = m_read_in_passes.size();
		}


		void setPhysicalIndex(uint32_t index)
		{
			m_physical_index = index;
		}
		


	private:

		ResourceType m_type;
		uint32_t m_index;
		unsigned m_physical_index = Unset;
		uint32_t m_read_in_pass_count; //refcount of pass reads
		std::unordered_set<uint32_t> m_written_in_passes;
		std::unordered_set<uint32_t> m_read_in_passes;
		std::string m_name;
		bool m_used_in_graph;
		//add more here
	};



	class RenderGraphImageResource : public RenderGraphResource
	{
	public:

		RenderGraphImageResource(uint32_t index) : RenderGraphResource(index, ResourceType::Image), m_usage(ImageUsage::None) {}

		void setImageInfo(const RenderGraphImageInfo& info)
		{
			if(m_info.properties.imageSize().width == 0) 
			{ 
				m_info = info;
			}
		}

		void addImageUsage(ImageUsage usage)
		{
			m_usage = m_usage | usage;
		}



		ImageUsage imageUsage() const {return m_usage;};
		ImageProperties imageProperties() const {return m_info.properties;};
		ImageViewType imageViewType() const {return m_info.view_type;};

	private:

		RenderGraphImageInfo m_info;
		ImageUsage m_usage;
	};




	class RenderGraphBufferResource : public RenderGraphResource
	{
	public:

		RenderGraphBufferResource(uint32_t index) : RenderGraphResource(index, ResourceType::Buffer) {}

		void setBufferInfo(const RenderGraphBufferInfo& info)
		{
			m_info = info;
		}

		void setBufferUsage(ShaderBufferUsage usage)
		{
			m_usage = usage;
		}

		uint32_t bufferSize() const {return m_info.size;}; //Returns size of subbuffer
		uint32_t subBufferCount() const { return m_info.subbuffer_count;};
		ShaderBufferUsage bufferUsage() const {return m_usage;};
	private:

		RenderGraphBufferInfo m_info;
		ShaderBufferUsage m_usage;

	};

	




}
