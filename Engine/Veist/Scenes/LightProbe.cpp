#include "pch.h"

#include "LightProbe.h"

#include "Veist/Graphics/RenderModule.h"


namespace Veist
{


static VkImageView getMipmapImageView(uint32_t mip_level, const ImageBase& image)
{
	VkImageViewCreateInfo img_view_info = {};
	img_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	img_view_info.pNext = nullptr;

	img_view_info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	img_view_info.image = image.image();
	img_view_info.format = VkFormat(image.properties().imageFormat().format());
	img_view_info.subresourceRange.baseMipLevel = mip_level;
	img_view_info.subresourceRange.levelCount = 1;
	img_view_info.subresourceRange.baseArrayLayer = 0;
	img_view_info.subresourceRange.layerCount = 6;
	img_view_info.subresourceRange.aspectMask = image.properties().imageFormat().imageAspectFlags();

	VkDevice device = RenderModule::getBackend()->getDevice();

	VkImageView view = {};
	vkCreateImageView(device, &img_view_info, nullptr, &view);
	return view;
}


static std::shared_ptr<Cubemap> calculateIrradianceMap(const Cubemap& HDRcubemap, uint32_t map_size)
{
	ComputePipeline compute_irradiance = {"IBLIrradianceMap"};

	ImageProperties cubemap_properties = { {map_size, map_size}, HDRcubemap.properties().imageFormat(), 1, 6 }; //32x32 cubemap
	StorageCubemap cubemap{cubemap_properties}; 
	

	std::vector<Descriptor> bindings;
	bindings.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &HDRcubemap, SamplerType::RepeatLinear);
	bindings.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, &cubemap);
	//compute_descriptor.setDescriptorSetLayout(0, &compute_irradiance);
	//compute_descriptor.bindCombinedSamplerImage(0, &HDRcubemap, { SamplerType::RepeatLinear });
	//compute_descriptor.bindStorageImage(1, &cubemap);
	//compute_descriptor.buildDescriptorSet();
	DescriptorSet compute_descriptor{0, bindings};
	
	CommandBuffer cmd_buff = RenderModule::getBackend()->createDisposableCmdBuffer();
	
	cmd_buff.calcSizeAndDispatch(compute_irradiance, compute_descriptor, cubemap_properties.imageSize());
	cmd_buff.immediateSubmit(RenderModule::getBackend()->getGraphicsQueue());

	cubemap.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

	return std::make_unique<Cubemap>(std::move(cubemap));
}




static std::shared_ptr<Cubemap> calculateEnvironmentMap(const Cubemap& HDRcubemap, uint32_t map_size)
{
	
	uint32_t max_mips = std::floor(std::log2(HDRcubemap.properties().imageSize().width)) + 1;
	uint32_t mip_levels = 5; //calculate mip levels for env map
	assert(max_mips > mip_levels);

	ComputePipeline compute_IBLenvironment = { "IBLEnvironmentMap" };
	
	ImageProperties cubemap_properties = { {map_size, map_size}, HDRcubemap.properties().imageFormat(), mip_levels, 6 }; //32x32 cubemap
	StorageCubemap cubemap{cubemap_properties};
	
	CommandBuffer cmd_buff = RenderModule::getBackend()->createDisposableCmdBuffer();

	glm::u32vec3 size;
	size.x = cubemap_properties.imageSize().width;
	size.y = cubemap_properties.imageSize().height;
	size.z = 1;

	std::vector<VkImageView> mipmap_views;
	std::vector<DescriptorSet> compute_descriptors;
	compute_descriptors.reserve(mip_levels);
	for (uint32_t i = 0; i < mip_levels; i++)
	{
		
		mipmap_views.push_back(getMipmapImageView(i, cubemap));
		
		std::vector<Descriptor> bindings;
		bindings.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &HDRcubemap, SamplerType::RepeatLinear);
		
		VkDescriptorImageInfo desc_info;
		desc_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		desc_info.imageView = mipmap_views.back();
		bindings.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, Descriptor::DescriptorInfo(desc_info));


		

		//compute_descriptor.setDescriptorSetLayout(0, &compute_IBLenvironment);
		//compute_descriptor.bindCombinedSamplerImage(0, &HDRcubemap, { SamplerType::RepeatLinear });
		//compute_descriptor.bindStorageImage(1, mipmap_views.back());
		//compute_descriptor.buildDescriptorSet();
		compute_descriptors.emplace_back(0, bindings );

		//ROUGHNESS PUSH CONSTNAT
		float roughness = (float)i / (float)(mip_levels - 1);

		cmd_buff.setComputePushConstant(compute_IBLenvironment, roughness);
		cmd_buff.calcSizeAndDispatch(compute_IBLenvironment, compute_descriptors[i], size);
		
		size.x /= 2;
		size.y /= 2;
	}

	cmd_buff.immediateSubmit(RenderModule::getBackend()->getGraphicsQueue());

	cubemap.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	

	VkDevice device = RenderModule::getBackend()->getDevice();
	for (auto view : mipmap_views)
	{
		vkDestroyImageView(device, view, nullptr);
	}

	return std::make_unique<Cubemap>(std::move(cubemap));
}


static std::shared_ptr<Texture> calculateBRDF_LUT(const Cubemap& HDRcubemap, uint32_t map_size)
{

	ComputePipeline compute_BRDF = { "IntegrateBRDF" };

	ImageProperties texture_properties = { {map_size, map_size}, HDRcubemap.properties().imageFormat(), 1, 1 }; 
	StorageTexture brdf_lut{ texture_properties };

	
	std::vector<Descriptor> bindings;
	bindings.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, &brdf_lut);
	//compute_descriptor.setDescriptorSetLayout(0, &compute_BRDF);
	//compute_descriptor.bindStorageImage(0, &brdf_lut);
	//compute_descriptor.buildDescriptorSet();
	DescriptorSet compute_descriptor{ 0, bindings };



	CommandBuffer cmd_buff = RenderModule::getBackend()->createDisposableCmdBuffer();

	cmd_buff.calcSizeAndDispatch(compute_BRDF, compute_descriptor, texture_properties.imageSize());
	cmd_buff.immediateSubmit(RenderModule::getBackend()->getGraphicsQueue());

	brdf_lut.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

	return std::make_unique<Texture>(std::move(brdf_lut));

}




LightProbe::LightProbe(const Cubemap& HDRcubemap, uint32_t irradiance_size, uint32_t environment_size, uint32_t brdf_size ) :
	m_irr_map_size(irradiance_size), m_env_map_size(environment_size), m_brdf_LUT_size(brdf_size)
{
	
	//TODO: create light probe

	// irradiance map convolution
	m_irradiance = calculateIrradianceMap(HDRcubemap, m_irr_map_size);


	// environment map roughness mipmaps
	m_environment_map = calculateEnvironmentMap(HDRcubemap, m_env_map_size);


	// BRDF LUT
	m_brdf_LUT = calculateBRDF_LUT(HDRcubemap, m_brdf_LUT_size);


}


}