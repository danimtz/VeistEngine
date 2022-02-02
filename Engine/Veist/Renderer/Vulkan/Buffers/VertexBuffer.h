#pragma once

#include "vulkan/vulkan.h"
#include "Veist/Renderer/Vulkan/Buffers/Buffer.h"
#include "Veist/Renderer/Vulkan/Buffers/VertexDescription.h"



namespace Veist
{

class VertexBuffer : public Buffer
{
public:

	VertexBuffer(void* vertices, uint32_t size);


	
	const VertexDescription& getInputDescription() const  { return m_description; }
	void setInputDescription(const VertexDescription& description) { m_description = description; };


private:

	VertexDescription	m_description;

};

}