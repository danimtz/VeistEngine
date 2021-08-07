#pragma once


#include <memory>
#include <vector>
#include <string>

#include "Engine/Logger.h"
#include "Engine/Renderer/Buffers/UniformBuffer.h"

class DescriptorHandler
{
public:

	virtual ~DescriptorHandler(){};

	virtual void writeUniformBuffer(UniformBuffer *buffer, uint32_t binding) = 0;
	virtual void updateDescriptorSet() = 0;



	static std::shared_ptr<DescriptorHandler> Create();



};

