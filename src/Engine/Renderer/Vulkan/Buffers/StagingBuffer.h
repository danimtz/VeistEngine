#pragma once

#include "vulkan/vulkan.h"
#include "Engine/Renderer/Vulkan/Buffers/Buffer.h"
#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"
#include "Engine/Logger.h"



class StagingBuffer : public Buffer
{
public:

	StagingBuffer(void* data, uint32_t size);
	~StagingBuffer();

};
