#pragma once

#include "Logger.h"

#include "Renderer/Buffer.h"


namespace AssetLoader {


	//This will need to be expanded upon. for now just load a mesh from a gltf file
	struct MeshInfo{

		std::vector<unsigned char> vtx_buffer_data; //Vertex buffer data. SHOULD I CONVER THIS TO AN IN ENGINE READABLE FORMAT FIRST? OR JUST UPLOAD TO VkBuffer LIKE THIS
		std::vector<unsigned char> idx_buffer_data;
		VertexDescription description;

	};

	MeshInfo& loadMeshFromGLTF(const char* filepath);



}

