#pragma once


#include <vulkan/vulkan.h>
#include "Veist/Graphics/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Veist/Graphics/Vulkan/ShaderAndPipelines/ComputePipeline.h"
#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Buffers/ShaderBuffer.h"

namespace Veist
{

	class Descriptor
	{
	public:

		struct DescriptorInfo
		{
			VkDescriptorBufferInfo buffer_info = {};
			VkDescriptorImageInfo image_info = {};

			DescriptorInfo() = default;

			DescriptorInfo(VkDescriptorBufferInfo buff) : buffer_info(buff)
			{}

			DescriptorInfo(VkDescriptorImageInfo img) : image_info(img)
			{}
		};

		Descriptor(VkDescriptorType type, DescriptorInfo info, VkShaderStageFlags stage_flag = VK_SHADER_STAGE_ALL) : m_type(type), m_info(info), m_stage_flag(stage_flag){};
		
		Descriptor(VkDescriptorType type, const ImageBase* image, SamplerType sampler_type = SamplerType::None, 
					VkShaderStageFlags stage_flag = VK_SHADER_STAGE_ALL, ImageUsage pass_usage = ImageUsage::None);

		Descriptor(VkDescriptorType type, const ShaderBuffer* buffer,  uint32_t range = 0, VkShaderStageFlags stage_flag = VK_SHADER_STAGE_ALL);
		
		VkDescriptorType type() const { return m_type;};
		VkShaderStageFlags stageFlags() const { return m_stage_flag; };
		DescriptorInfo& info() { return m_info; };

	private:
		
		DescriptorInfo m_info;
		VkDescriptorType m_type;
		VkShaderStageFlags m_stage_flag;
	
	};


	class DescriptorSetPool;

	class DescriptorSet
	{
	public:
		struct DescriptorPoolData
		{
			uint32_t m_index; //index in pool in which descriptor set is found
			DescriptorSetPool* m_pool; //Which pool corresponds to descriptor set layout
		};

		DescriptorSet(uint32_t set_number, std::vector<Descriptor>& descriptor_bindings);
		~DescriptorSet();

		uint32_t setNumber() const {return m_set_number;};
		VkDescriptorSet descriptorSet() const;
		VkDescriptorSetLayout descriptorSetLayout() const;

	private:

		uint32_t m_set_number;
		DescriptorPoolData m_pool_data;
		
	};


/*
	class ShaderBuffer;



	class Descriptor
	{
	public:

		struct DescriptorInfo
		{
			VkDescriptorBufferInfo buffer_info = {};
			VkDescriptorImageInfo image_info = {};

			
			DescriptorInfo() = default;

			DescriptorInfo(VkDescriptorBufferInfo buff) : buffer_info(buff)
			{}

			DescriptorInfo(VkDescriptorImageInfo img) : image_info(img)
			{}


		};


		//Image descriptor
		template<ImageViewType type>
		Descriptor(const Image<ImageUsage::Texture, type>& image, const Sampler sampler) : m_type(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{

			VkDescriptorImageInfo desc_img_info;
			desc_img_info.imageLayout = getImageLayout(image.imageUsage());
			desc_img_info.imageView = image.imageView();
			desc_img_info.sampler = sampler.sampler();
			
			m_info = {desc_img_info};
			
		}

		template<ImageViewType type>
		Descriptor(const Image<ImageUsage::Texture | ImageUsage::Storage, type>& image, const Sampler sampler) : m_type(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
		{

			VkDescriptorImageInfo desc_img_info;
			desc_img_info.imageLayout = getImageLayout(ImageUsage::Storage);
			desc_img_info.imageView = image.imageView();
			desc_img_info.sampler = sampler.sampler();
			m_info = { desc_img_info };


		}


		Descriptor(VkDescriptorType type, DescriptorInfo info = DescriptorInfo()) : m_type(type), m_info(info)
		{}


		void setDescriptorInfo()
		{

		}


		VkDescriptorType type() const { return m_type; }; 
		DescriptorInfo info() const { return m_info; };

	private:

		VkDescriptorType m_type;

		DescriptorInfo m_info;
		

	};



	class DescriptorSet
	{
	public:
	
		DescriptorSet() = default;
		void setDescriptorSetLayout(uint32_t set, const GraphicsPipelineBuilder* pipeline);
		void setDescriptorSetLayout(uint32_t set, const ComputePipeline* pipeline); //TODO: maybe make compute and graphics pipeline inherit from pipeline? maybe not though


		//TODO maybe replace all these with bind descriptor
		void bindUniformBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range);
		void bindStorageBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range);
		void bindCombinedSamplerImage(uint32_t binding, const Texture* texture, const Sampler sampler);
		void bindCombinedSamplerImage(uint32_t binding, const Cubemap* cubemap, const Sampler sampler);
		void bindCombinedSamplerImage(uint32_t binding, const ImageBase* image, const Sampler sampler);
		void bindStorageImage(uint32_t binding, const ImageBase* image);
		void bindStorageImage(uint32_t binding, const VkImageView image_view);
	
		void bindDescriptor(uint32_t binding, const Descriptor desc);


		void buildDescriptorSet();
		void updateDescriptorSet();

		uint32_t setNumber() const { return m_set_number; };

		VkDescriptorSet& descriptorSet() { return m_descriptor_set; };
		const VkDescriptorSet& descriptorSet() const { return m_descriptor_set; };
	

		VkDescriptorSetLayout descriptorSetLayout() const { return m_descriptor_layout; };

	private:
		//void bindBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range, VkDescriptorType type);
		void bindSampledImage(uint32_t binding, VkImageView image_view, VkDescriptorType type, VkSampler sampler); //Sampler created inside this function. should be external argument i reckon
	

		uint32_t m_set_number;

		VkDescriptorSetLayout m_descriptor_layout = nullptr;
		VkDescriptorSet m_descriptor_set = nullptr;
	
		std::vector<VkWriteDescriptorSet> m_writes;

	

		std::vector<Descriptor::DescriptorInfo> m_write_data;
	};

*/
}