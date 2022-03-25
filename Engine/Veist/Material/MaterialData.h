#pragma once

#include "Veist/Graphics/Vulkan/Images/Image.h"


namespace Veist
{


	class MaterialData
	{
	public:
		MaterialData() = default;
		MaterialData(std::vector<std::shared_ptr<ImageBase>> textures) : m_textures(textures) {}
		
		const std::vector<std::shared_ptr<ImageBase>>& textures() const {return m_textures;};

	protected:
		
		//These should be resource pointers once resource system is implemented. FOr now they are stored here with shared pointers
		std::vector<std::shared_ptr<ImageBase>> m_textures;

		/*
		enum PBRTextures : uint32_t
		{
			Albedo = 0,
			Normal = 1,
			OcclusionRoughnessMetallic = 2,
			Emmissive = 3
		};*/
	};

		
}