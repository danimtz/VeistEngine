#pragma once

#include "vulkan/vulkan.h"
#include "Veist/Renderer/Vulkan/Buffers/Buffer.h"
#include "Veist/Renderer/Vulkan/Buffers/VertexDescription.h"



namespace Veist
{

class StagingBuffer : public Buffer
{
public:

	StagingBuffer(void* data, uint32_t size);
	~StagingBuffer();

};

}