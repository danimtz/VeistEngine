#pragma once

#include <memory>
#include <vector>
#include <string>

#include "Engine/Logger.h"


class UniformBuffer
{
public:

	virtual ~UniformBuffer(){};

	virtual void* getBuffer() const = 0;
	virtual uint32_t getSize() const = 0;

	virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

	static std::shared_ptr<UniformBuffer> Create(uint32_t size);


};

