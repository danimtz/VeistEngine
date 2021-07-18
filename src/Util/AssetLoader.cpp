

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include "AssetLoader.h"

AssetLoader::MeshInfo& AssetLoader::loadMeshFromGLTF(const char* filepath) {
	
	

	//Load TinyGLTF model
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err;
	std::string warn;
	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
	if(!warn.empty()){
		CRITICAL_ERROR_LOG(warn);
	}
	if (!err.empty()){
		CRITICAL_ERROR_LOG(err);
	}
	if (!res){
		CRITICAL_ERROR_LOG("Failed to load gltf file:" + std::string(filepath))
	} else {
		CONSOLE_LOG("Loaded: " + std::string(filepath))
	}


	//TODO extract vertex description and vertex and index buffers from TinyGLTF model.


	MeshInfo mesh_info;


	//TEMPORARY. REMOVE SOON: THIS SHOULD BE OBTAINED FROM model
	mesh_info.idx_buffer_data.push_back(0x01);
	mesh_info.vtx_buffer_data.push_back(0x02);
	mesh_info.vtx_buffer_data.push_back(0x02);
	mesh_info.description = { 0, {			
		{VertexAttributeType::Float3, "position"},
		{VertexAttributeType::Float3, "normal"},
		{VertexAttributeType::Float3, "color"} } };

	return mesh_info;


};
