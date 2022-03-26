#pragma once


#include <Veist/Graphics/RenderModule.h>
#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Buffers/ShaderBuffer.h"

namespace Veist
{

	struct RenderGraphBufferInfo
	{
		uint32_t size;
		uint32_t subbuffer_count;
	};

	struct RenderGraphImageInfo
	{
		ImageProperties properties;
	};


	
	class RenderGraphResource
	{
	public:

		RenderGraphResource(uint32_t index) : m_index(index) {};

		uint32_t index() const { return m_index; }

		const std::unordered_set<uint32_t>& writtenInPasses() const { return m_written_in_passes; }

		const std::unordered_set<uint32_t>& readInPasses() const { return m_read_in_passes; }

		uint32_t& passReadsRefCount() {return m_read_in_pass_count;};

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
		}


		


	private:

		uint32_t m_index;
		uint32_t m_read_in_pass_count; //refcount of pass reads
		std::unordered_set<uint32_t> m_written_in_passes;
		std::unordered_set<uint32_t> m_read_in_passes;
		std::string m_name;
		//add more here
	};



	class RenderGraphImageResource : public RenderGraphResource
	{
	public:

		RenderGraphImageResource(uint32_t index) : RenderGraphResource(index) {}

		void setImageInfo(const RenderGraphImageInfo& info)
		{
			m_info = info;
		}

		void setImageUsage(ImageUsage usage)
		{
			m_usage = usage;
		}

	private:

		RenderGraphImageInfo m_info;
		ImageUsage m_usage;
	};




	class RenderGraphBufferResource : public RenderGraphResource
	{
	public:

		RenderGraphBufferResource(uint32_t index) : RenderGraphResource(index) {}

		void setBufferInfo(const RenderGraphBufferInfo& info)
		{
			m_info = info;
		}

		void setBufferUsage(ShaderBufferUsage usage)
		{
			m_usage = usage;
		}

	private:

		RenderGraphBufferInfo m_info;
		ShaderBufferUsage m_usage;

	};

	


	template<ImageViewType type = ImageViewType::Flat>
	class TransientImage : public ImageBase
	{
	public:

		TransientImage(ImageProperties properties, ImageUsage usage) : ImageBase(properties, usage, type) {};

		TransientImage() = default;


		//Add c++ rule of 5 functions?
	};



}
