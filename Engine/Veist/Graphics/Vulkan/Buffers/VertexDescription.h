#pragma once

//#include <memory>
//#include <vector>
//#include <string>
#include <glm/glm.hpp>

#include "Veist/Util/Logger.h"


namespace Veist
{


	#pragma pack(push, 1)
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 tangent;
		glm::vec2 uv;
	};
	#pragma pack(pop)


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
			default: CRITICAL_ERROR_LOG("Invalid VertexAttributeType");
		}
	}



	struct VertexAttribute
	{
		uint32_t m_size{0};
		uint32_t m_offset{0};
		std::string m_name;
		VertexAttributeType m_type;

		VertexAttribute() = default;
		VertexAttribute(VertexAttributeType type, std::string name) : m_size(VertexAttributeTypeSize(type)), m_name(name), m_type(type) {};
		//VertexAttribute(uint32_t attribute_size, std::string name) : m_size(attribute_size), m_name(name), m_type(type) {};
	};




	//API independent vertex input description
	class VertexDescription
	{
	public:

		VertexDescription() = default;

		//Custom vertex constructor
		VertexDescription(uint32_t binding, std::initializer_list<VertexAttribute> attributes) : m_binding(binding), m_attributes(attributes) { calculateStrideAndOffsets(); };

		//From GLTF vertex description
		VertexDescription(uint32_t binding, std::vector<VertexAttribute> &attributes) : m_binding(binding), m_attributes(attributes) { calculateStrideAndOffsets(); };

		uint32_t getStride() const { return m_stride; };
		uint32_t getBinding() const { return m_binding; };
		const std::vector<VertexAttribute>& getVertexAttributes() const { return m_attributes; };

	private:

		void calculateStrideAndOffsets(){

			uint32_t offset = 0;
			m_stride = 0;

			for(auto &attribute : m_attributes){
			
				attribute.m_offset = offset;
				offset += attribute.m_size;
				m_stride += attribute.m_size;
		
			}
		};

		uint32_t m_binding; //maybe a static id but i dont think so
		uint32_t m_stride;
		std::vector<VertexAttribute> m_attributes;
	};


}