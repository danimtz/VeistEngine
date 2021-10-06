#pragma once

#include "CommandPool.h"

#include <vulkan/vulkan.h>

class CommandBuffer
{
public:

	CommandBuffer() = default;


	void submit(){};


private:

	std::shared_ptr<CommandPool> m_command_pool;

};

