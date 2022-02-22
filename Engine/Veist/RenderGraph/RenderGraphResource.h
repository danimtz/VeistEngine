#pragma once


#include <Veist/Graphics/RenderModule.h>


namespace Veist
{

	struct RenderGraphBufferInfo
	{

		uint32_t size;

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

	};



	class RenderGraphTransientImage : public RenderGraphResource
	{
	public:




	private:

		RenderGraphImageInfo m_info;

	};


	class RenderGraphTransientBuffer : public RenderGraphResource
	{
	public:



	private:

		RenderGraphBufferInfo m_info;


	};

	


}
