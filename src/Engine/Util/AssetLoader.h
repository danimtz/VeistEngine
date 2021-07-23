#pragma once

#include "Engine/Logger.h"

#include "Engine/Renderer/Buffers/Buffer.h"


namespace AssetLoader {


	//This will need to be expanded upon. for now just load a mesh from a gltf file
	struct MeshInfo{

		std::vector<unsigned char> vbuffer_data; //Vertex buffer data. 
		std::vector<unsigned char> index_data;
		uint32_t index_count;
		uint32_t index_size = {2};
		VertexDescription description;

	};

	void loadMeshFromGLTF(const char* filepath, MeshInfo &mesh_info);



}

