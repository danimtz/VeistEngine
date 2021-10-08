#pragma once

#include "vulkan/vulkan.h"
#include "Engine/Renderer/Vulkan/Buffers/Buffer.h"
#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"
#include "Engine/Logger.h"



class VertexBuffer : public Buffer
{
public:

	VertexBuffer(void* vertices, uint32_t size);


	
	const VertexDescription& getInputDescription() const  { return m_description; }
	void setInputDescription(const VertexDescription& description) { m_description = description; };


private:

	VertexDescription	m_description;

};
