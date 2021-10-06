

#include "RenderPass.h"
#include "Engine/Renderer/RenderModule.h"




static VkImageLayout getImageLayout(ImageUsage usage) {

	switch (usage) //If usage only has one flag then use this
	{

		case ImageUsage::ColorAttachment:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		case ImageUsage::DepthAttachment:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		case ImageUsage::Texture:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		case ImageUsage::SwapchainImage:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		case ImageUsage::TransferSrc:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		case ImageUsage::TransferDst:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		default:
			break;
	}

	//If additional flags present bitwise and to remove them
	if ((usage & ImageUsage::SwapchainImage) != ImageUsage::None) {
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}

	if ((usage & ImageUsage::DepthAttachment) != ImageUsage::None) {
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	if ((usage & ImageUsage::ColorAttachment) != ImageUsage::None) {
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	if ((usage & ImageUsage::Texture) != ImageUsage::None) {
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	


	CRITICAL_ERROR_LOG("Unsuported image layout");
}


static VkAttachmentDescription createAttachmentDescription(RenderPass::AttachmentProperties& attachment) 
{
	//Setup depth attachment

	VkImageLayout initial_layout = getImageLayout(attachment.usage & (~ImageUsage::SwapchainImage)); //layout without swapchain bit
	VkImageLayout final_layout = getImageLayout(attachment.usage);
	
	
	//If usage is swapchain and depth attachment, set final layout to depth stencil and initial to undefined
	if ((attachment.usage & (ImageUsage::DepthAttachment | ImageUsage::SwapchainImage)) == attachment.usage)
	{
		initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	//If renderpass being used for swapchain present, set initial layout is undefined
	if (final_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	}
	

	VkAttachmentDescription description = {};
	description.flags = 0;
	description.format = attachment.properties.imageFormat().format();
	description.samples = VK_SAMPLE_COUNT_1_BIT;

	description.loadOp = VkAttachmentLoadOp(attachment.load_op);
	description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	VkAttachmentLoadOp stencil_load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	if ((attachment.usage & ImageUsage::DepthAttachment) != ImageUsage::None) 
	{
		stencil_load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
	} 
	description.stencilLoadOp = stencil_load_op;
	description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	description.initialLayout = initial_layout;
	description.finalLayout = final_layout;
	 
	return description;
}

static VkAttachmentReference createAttachmentReference(uint32_t index, ImageUsage usage)
{
	//Get layout of attachment reference (must be depth or color attachment)
	VkImageLayout layout = getImageLayout(usage & (ImageUsage::DepthAttachment | ImageUsage::ColorAttachment));

	//Ref to depth attchment
	VkAttachmentReference reference = {};
	reference.attachment = index;
	reference.layout = layout;
	
	return reference;
}


static VkSubpassDependency createSubpassDependency(bool has_depth) //TODO: depencencies dont really work well atm
{
	VkAccessFlags access_mask = 0;
	VkPipelineStageFlags stage_mask = 0;

	if (has_depth) {
		stage_mask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		access_mask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	
	stage_mask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	access_mask |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;;
	dependency.dstStageMask = stage_mask;

	dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
	dependency.dstAccessMask = access_mask;//VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


	return dependency;
}


RenderPass::RenderPass(std::vector<AttachmentProperties>& color_properties, AttachmentProperties& depth_properties)
{

	bool has_depth_attachment;
	if (depth_properties.usage == ImageUsage::None)
	{
		has_depth_attachment = false;
	}
	else if((depth_properties.usage & ImageUsage::DepthAttachment) == ImageUsage::DepthAttachment)
	{
		has_depth_attachment = true;
	}
	else
	{
		CRITICAL_ERROR_LOG("Renderpass depth attachment has incorrect image usage bit");
	}

	uint32_t color_attachment_count = color_properties.size();

	std::vector<VkAttachmentDescription> attachment_descriptions;
	std::vector<VkAttachmentReference> attachment_references;
	attachment_descriptions.reserve(color_attachment_count + 1);
	attachment_references.reserve(color_attachment_count + 1);

	//Color attachments
	for(uint32_t i = 0; i < color_attachment_count; i++)
	{
		attachment_descriptions.push_back(createAttachmentDescription(color_properties[i]));
		attachment_references.push_back(createAttachmentReference(i, color_properties[i].usage));

	}

	if(has_depth_attachment)
	{
		//Depth attachment
		attachment_descriptions.push_back(createAttachmentDescription(depth_properties));
		attachment_references.push_back(createAttachmentReference(color_properties.size(), depth_properties.usage));

	}
	
	//Create subpass
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = color_attachment_count;
	subpass.pColorAttachments = attachment_references.data();

	if (has_depth_attachment)
	{
		subpass.pDepthStencilAttachment = &attachment_references[color_attachment_count];
	}


	VkSubpassDependency dependency = createSubpassDependency(has_depth_attachment);


	//Create renderpass
	VkRenderPassCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	create_info.attachmentCount = attachment_descriptions.size();
	create_info.pAttachments = attachment_descriptions.data();
	create_info.subpassCount = 1; 
	create_info.pSubpasses = &subpass;
	create_info.dependencyCount = 1;
	create_info.pDependencies = &dependency;

	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	
	VK_CHECK(vkCreateRenderPass(device, &create_info, nullptr, &m_render_pass));
	
	VkRenderPass renderpass = m_render_pass;

	if ((color_properties[0].usage & ImageUsage::SwapchainImage) != ImageUsage::None)
	{
		RenderModule::getRenderBackend()->pushToSwapchainDeletionQueue([device, renderpass]() { vkDestroyRenderPass(device, renderpass, nullptr);	});
	}
	else 
	{
		RenderModule::getRenderBackend()->pushToDeletionQueue([device, renderpass]() { vkDestroyRenderPass(device, renderpass, nullptr);	});

	}
	

}


RenderPass::RenderPass(std::vector<AttachmentProperties>& color_properties) : 
	RenderPass(color_properties, AttachmentProperties()) {};//Renderpass without depth attachment
