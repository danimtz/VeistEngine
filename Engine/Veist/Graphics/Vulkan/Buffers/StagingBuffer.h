#pragma once

#include "vulkan/vulkan.h"
#include "Veist/Graphics/Vulkan/Buffers/Buffer.h"
#include "Veist/Graphics/Vulkan/Buffers/VertexDescription.h"



namespace Veist
{

class StagingBuffer : public Buffer
{
public:

	StagingBuffer(void* data, uint32_t size);
	~StagingBuffer();

};

}