
#include "LightProbe.h"

#include "Engine/Renderer/RenderModule.h"




static std::unique_ptr<Cubemap> calculateIrradianceMap(const Cubemap& HDRcubemap, uint32_t map_size)
{
	ComputePipeline compute_irradiance = {"IrradianceMap"};

	ImageProperties cubemap_properties = { {map_size, map_size}, HDRcubemap.properties().imageFormat(), 1, 6 }; //32x32 cubemap
	StorageCubemap cubemap{cubemap_properties}; 
	
	DescriptorSet compute_descriptor;
	compute_descriptor.setDescriptorSetLayout(0, &compute_irradiance);
	compute_descriptor.bindCombinedSamplerCubemap(0, &HDRcubemap);
	compute_descriptor.bindStorageImage(1, &cubemap);
	compute_descriptor.buildDescriptorSet();

	CommandBuffer cmd_buff = RenderModule::getRenderBackend()->createDisposableCmdBuffer();
	
	cmd_buff.calcSizeAndDispatch(compute_irradiance, compute_descriptor, cubemap_properties.imageSize());
	cmd_buff.immediateSubmit();

	cubemap.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

	return std::make_unique<Cubemap>(std::move(cubemap));
}




static std::unique_ptr<Cubemap> calculateEnvironmentMap(const Cubemap& HDRcubemap, uint32_t map_size)
{
	
	uint32_t mip_levels = 4; //calculate mip levels for env map

	ComputePipeline compute_IBLenvironment = { "IBLEnvironmentMap" };
	
	ImageProperties cubemap_properties = { {map_size, map_size}, HDRcubemap.properties().imageFormat(), mip_levels, 6 }; //32x32 cubemap
	StorageCubemap cubemap{cubemap_properties};

	
	DescriptorSet compute_descriptor;
	compute_descriptor.setDescriptorSetLayout(0, &compute_IBLenvironment);
	compute_descriptor.bindCombinedSamplerCubemap(0, &HDRcubemap);
	compute_descriptor.bindStorageImage(1, &cubemap);

	CommandBuffer cmd_buff = RenderModule::getRenderBackend()->createDisposableCmdBuffer();

	cmd_buff.calcSizeAndDispatch(compute_IBLenvironment, compute_descriptor, cubemap_properties.imageSize());
	cmd_buff.immediateSubmit();

	cubemap.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	
	return std::make_unique<Cubemap>(std::move(cubemap));
}


static std::unique_ptr<Texture> calculateBRDF_LUT(const Cubemap& HDRcubemap)
{





	return std::unique_ptr<Texture>();
}




LightProbe::LightProbe(const Cubemap& HDRcubemap, uint32_t irradiance_size, uint32_t environment_size) : m_irr_map_size(irradiance_size), m_env_map_size(environment_size)
{
	
	//TODO: create light probe

	// irradiance map convolution
	m_irradiance = calculateIrradianceMap(HDRcubemap, m_irr_map_size);


	// environment map roughness mipmaps
	m_environment_map = calculateEnvironmentMap(HDRcubemap, m_env_map_size);


	// BRDF LUT
	m_brdf_LUT = calculateBRDF_LUT(HDRcubemap);


}
