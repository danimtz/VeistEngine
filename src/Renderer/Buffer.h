#pragma once

#include <memory>
#include <vector>
#include <string>

enum class VertexAttributeType {
	None = 0, Float, Float2, Float3, Float4 /*, Mat3, Mat4, Int, Int2, Int3, Int4, Bool*/
};


static uint32_t VertexAttributeTypeSize(VertexAttributeType type)
{
	switch (type) {
												//Bytes * count 
		case VertexAttributeType::Float:	return 4;
		case VertexAttributeType::Float2:	return 4 * 2;
		case VertexAttributeType::Float3:	return 4 * 3;
		case VertexAttributeType::Float4:	return 4 * 4;
		
		/*
		case VertexAttributeType::Mat3:		return 4 * 3 * 3;
		case VertexAttributeType::Mat4:		return 4 * 4 * 4;

		case VertexAttributeType::Int:		return 4;
		case VertexAttributeType::Int2:		return 4 * 2;
		case VertexAttributeType::Int3:		return 4 * 3;
		case VertexAttributeType::Int4:		return 4 * 4;

		case VertexAttributeType::Bool:		return 1;*/
	}
}

struct VertexAttribute
{
	uint32_t m_size;
	uint32_t m_offset = {0};
	std::string m_name;
	VertexAttributeType m_type;

	VertexAttribute(VertexAttributeType type, std::string name) : m_size(VertexAttributeTypeSize(type)), m_name(name), m_type(type){};

};


class VertexDescription
{
public:

	VertexDescription() = default;
	VertexDescription(uint32_t binding, std::initializer_list<VertexAttribute> attributes) : m_binding(binding), m_attributes(attributes) { calculateStrideAndOffsets(); };

	uint32_t getStride() const { return m_stride; };
	uint32_t getBinding() const { return m_binding; };
	const std::vector<VertexAttribute>& getVertexAttrributes() const { return m_attributes; };

private:

	void calculateStrideAndOffsets(){

		size_t offset = 0;
		m_stride = 0;

		for(auto &attribute : m_attributes){
			
			attribute.m_offset = offset;
			offset += attribute.m_size;
			m_stride += attribute.m_size;
		
		}

	};
	uint32_t m_binding; //maybe a static id but i dont think so
	uint32_t m_stride = {0};
	std::vector<VertexAttribute> m_attributes;


};


class VertexBuffer
{
public:

	virtual ~VertexBuffer() = default;

	virtual void* getBuffer() const = 0;
	virtual uint32_t getSize() const= 0;
	//virtual void setInputDescription(const VertexDescription &description) = 0; MAYBE USE THIS??


	static std::shared_ptr<VertexBuffer> Create(void* vertices, uint32_t size);
	static std::shared_ptr<VertexBuffer> Create(uint32_t size);
};
