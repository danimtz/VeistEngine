#pragma once


#include <Veist/Graphics/RenderModule.h>
#include "Veist/Graphics/Vulkan/Images/Image.h"

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
		ImageUsage usage_flags;

	};

	
	class RenderGraphResource
	{
	public:





	private:

		uint32_t m_index;
		std::string m_name;
		//add more here
	};



	class RenderGraphImageResource : public RenderGraphResource
	{
	public:




	private:

		RenderGraphImageInfo m_info;
		//add more here
	};


	class RenderGraphBufferResource : public RenderGraphResource
	{
	public:



	private:

		RenderGraphBufferInfo m_info;
		//add more here

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
