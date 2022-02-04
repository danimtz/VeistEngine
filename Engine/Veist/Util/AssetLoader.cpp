#include "pch.h"


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <algorithm>
#include <map>
#include <memory>
#include "AssetLoader.h"

#include "Veist/Material/Material.h"
#include "Veist/Material/PBRMaterial.h"
#include "Veist/Material/SkyboxMaterial.h"

#include "Veist/Renderer/RenderModule.h"

namespace Veist
{

static VertexAttributeType convertTinyGLTFtype(int type) {

	switch (type) {
		case TINYGLTF_TYPE_VEC2: return VertexAttributeType::Float2;
		case TINYGLTF_TYPE_VEC3: return VertexAttributeType::Float3;
		case TINYGLTF_TYPE_VEC4: return VertexAttributeType::Float4;

		default: CRITICAL_ERROR_LOG("AssetLoader: Invalid Vertex type")
	}
}

static uint32_t getAttributeSlot(std::string attribute)
{
	if (!attribute.compare("POSITION")) 
	{
		return 0;
	}
	else if (!attribute.compare("NORMAL")) 
	{
		return 1;
	}
	else if(!attribute.compare("TEXCOORD_0")) 
	{
		return 3;
	}
	else if(!attribute.compare("TANGENT")) 
	{
		return 2;
	}
	else{ return 999;}//never get here
	
}

//maybe move this elsewhere
static void generateTangents(MeshData& mesh_data)
{
	
	
	//Iterate through each triangle
	auto& vertices = mesh_data.vbuffer_data;
	auto& indices = mesh_data.index_data;

	std::vector<glm::vec3> vert_tangents;
	std::vector<glm::vec3> vert_bitangents;
	std::vector<int> tangent_count; //used for averaging
	vert_tangents.resize(vertices.size());
	vert_bitangents.resize(vertices.size());
	tangent_count.resize(vertices.size());

	for (int i = 0; i < mesh_data.index_count; i += 3)
	{
		//Calculate tangents for each triangle
		glm::vec3 vert0 = vertices[indices[i + 0]].position;
		glm::vec3 vert1 = vertices[indices[i + 1]].position;
		glm::vec3 vert2 = vertices[indices[i + 2]].position;

		glm::vec2 texcoord0 = vertices[indices[i + 0]].uv;
		glm::vec2 texcoord1 = vertices[indices[i + 1]].uv;
		glm::vec2 texcoord2 = vertices[indices[i + 2]].uv;

		glm::vec3 edge1 = vert1 - vert0;
		glm::vec3 edge2 = vert2 - vert0;

		glm::vec2 deltaUV1 = texcoord1 - texcoord0;
		glm::vec2 deltaUV2 = texcoord2 - texcoord0;

		//r = 1 over determinant
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x); 

		glm::vec3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (edge2 * deltaUV1.x - edge1 * deltaUV2.x) * r;


		//check tangent handedness for each vertex THIS MADE HELMET TUBES FLIP INSIDE OUT
		/*for (int k = 0; k < 3; k++) {

			glm::vec3 normal = vertices[indices[i+k]].normal;

			glm::vec3 tan = tangent - (normal * glm::dot(normal, tangent));
			tan = glm::normalize(tan);
			glm::vec3 bitan = normalize(bitangent);

			glm::vec3 c = glm::cross(normal, tan);
			if (glm::dot(bitan, c) < 0.0f) {
				tangent = tangent * -1.0f;
			}

			vert_tangents[indices[i + k]] += tangent;

		}*/


		//Add tangent and bitangents to array at the indexed slot
		vert_tangents[indices[i + 0]] += tangent;
		vert_tangents[indices[i + 1]] += tangent;
		vert_tangents[indices[i + 2]] += tangent;

		vert_bitangents[indices[i + 0]] += bitangent;
		vert_bitangents[indices[i + 1]] += bitangent;
		vert_bitangents[indices[i + 2]] += bitangent;

		tangent_count[indices[i + 0]] += 1;
		tangent_count[indices[i + 1]] += 1;
		tangent_count[indices[i + 2]] += 1;
	}

	//Average tangents
	for (int i = 0; i < vertices.size(); i++) 
	{
		vert_tangents[i] = vert_tangents[i] * (1.0f / tangent_count[i]);
		vert_bitangents[i] = vert_bitangents[i] * (1.0f / tangent_count[i]);
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		//Gram-Schmidt orthogonalisation
		glm::vec3 n = vertices[i].normal;
		glm::vec3 t0 = vert_tangents[i];
		glm::vec3 b = vert_bitangents[i];
		
		glm::vec3 t = t0 - (n * glm::dot(n,t0));
		t = glm::normalize(t);

		float handedness = 1.0;
		//Correct handedness
		glm::vec3 c = glm::cross(n, t);
		if (glm::dot(b, c) < 0.0f) {
			handedness = -1.0f;
		}

		//Add tangent to vertex
		vertices[i].tangent = glm::vec4(t, handedness);
	}
}


std::shared_ptr<Mesh> AssetLoader::loadMeshFromGLTF(const char* gltf_filepath)
{
	
	
	MeshData mesh_data;
	//Load TinyGLTF model
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err;
	std::string warn;
	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, gltf_filepath);
	if(!warn.empty()){
		CRITICAL_ERROR_LOG(warn);
	}
	if (!err.empty()){
		CRITICAL_ERROR_LOG(err);
	}
	if (!res){
		CRITICAL_ERROR_LOG("Failed to load gltf file:" + std::string(gltf_filepath))
	} else {
		CONSOLE_LOG("Loaded: " + std::string(gltf_filepath))
	}


	
	tinygltf::Mesh &mesh = model.meshes[0];
	tinygltf::Primitive& primitive = mesh.primitives[0];
	
	
	//Step 1: Iterate through attributes in mesh primitive[0]
	struct AttributeInfo
	{
		AttributeInfo( tinygltf::Accessor accessor_in, VertexAttribute attribute) : accessor(accessor_in), vattribute(attribute) {};
		tinygltf::Accessor accessor;
		VertexAttribute vattribute;
	};

	std::map<int, AttributeInfo> vattrib_map;

	
	bool has_tangent = false;
	for (auto it = primitive.attributes.begin(); it != primitive.attributes.end(); it++ )
	{
		
		std::string attribute_name = it->first;
		
		if(!attribute_name.compare("TANGENT")) { has_tangent = true;};

		uint32_t attribute_slot = getAttributeSlot(attribute_name);

		//Get accessor referenced by primitive attribute
		tinygltf::Accessor accessor = model.accessors[it->second];

		//Create VertexAttribute for accessor and store it in map based on its corresponding bufferview
		VertexAttribute vtx_attrib = { convertTinyGLTFtype(accessor.type), attribute_name }; 
		
		AttributeInfo attrib_info = { accessor, vtx_attrib};

		vattrib_map.insert({ attribute_slot, attrib_info });
	}

	//Adds tangent if not in model
	if(!has_tangent)
	{
		VertexAttribute tangent_attrib = { VertexAttributeType::Float4, "TANGENT"};
		tinygltf::Accessor accessor;
		AttributeInfo attrib_info = { accessor, tangent_attrib };
		vattrib_map.insert({ getAttributeSlot("TANGENT"), attrib_info });
	}


	//Step 2: Create descriptions
	//Fill in vertex description
	std::vector<VertexAttribute> vertex_attributes;
	vertex_attributes.resize(vattrib_map.size());
	for (auto it = vattrib_map.begin(); it != vattrib_map.end(); it++) 
	{

		vertex_attributes[it->first] = it->second.vattribute;

	}

	VertexDescription vertex_desc = { 0, vertex_attributes };
	mesh_data.description = vertex_desc;



	//Step 3: Parse vertex data into interleaved format
	//std::vector<unsigned char>& vertex_buffer = mesh_data.vbuffer_data;//working variable
	std::vector<Vertex>& vertex_data = mesh_data.vbuffer_data;

	uint32_t stride = vertex_desc.getStride();
	uint32_t attribute_count = vattrib_map.begin()->second.accessor.count;
	uint32_t buffer_size = stride*attribute_count;
	mesh_data.vbuffer_size = buffer_size;
	//vertex_buffer.resize(buffer_size);

	vertex_data.resize(attribute_count);


	//Iterate through each vertex
	for (uint32_t i = 0; i < attribute_count; i++)
	{



		uint32_t location = 0;
		for (auto it = vattrib_map.begin(); it != vattrib_map.end(); it++) 
		{
				
			//if tangent not in buffer skip
			if(!has_tangent && location == 2) 
			{
				location++;
				continue;
			}

			uint32_t accessor_byte_offset = it->second.accessor.byteOffset;
			uint32_t buffer_view_byte_offset = model.bufferViews[it->second.accessor.bufferView].byteOffset;

			uint32_t buffer_offset = accessor_byte_offset + buffer_view_byte_offset;

			//acess_point = (Accessor type size or buffer view stride if stride is not equal to 0) times i
			uint32_t access_point =  i * ((model.bufferViews[it->second.accessor.bufferView].byteStride == 0) ? it->second.vattribute.m_size : model.bufferViews[it->second.accessor.bufferView].byteStride);


			//copy memory
			std::vector<unsigned char>& src_buffer = model.buffers[model.bufferViews[it->second.accessor.bufferView].buffer].data;
			//memcpy( &vertex_buffer.at(i*vertex_desc.getStride() + vertex_desc.getVertexAttributes()[location].m_offset), &src_buffer.at(buffer_offset+access_point), it->second.vattribute.m_size);
			
			Vertex* vertex_address = &vertex_data.at(i);
			char* dst_address = reinterpret_cast<char*>(vertex_address);
			dst_address += vertex_desc.getVertexAttributes()[location].m_offset;
			memcpy(dst_address, &src_buffer.at(buffer_offset + access_point), it->second.vattribute.m_size);


			location++;
		}
	}

	

	//Step 4: Get index buffer data
	int idx_buff_accessor_index = primitive.indices;
	tinygltf::Accessor idx_buff_accessor = model.accessors[idx_buff_accessor_index];
	mesh_data.index_count = idx_buff_accessor.count;
	

	
	//slice inndex buffer data
	uint32_t idx_buff_offset = model.bufferViews[idx_buff_accessor.bufferView].byteOffset;
	uint32_t idx_buff_size = model.bufferViews[idx_buff_accessor.bufferView].byteLength ; //byteLength should be equal to-> idx_buff_accessor.count * mesh_info.index_size
	mesh_data.index_data.resize(mesh_data.index_count);//resize to number of indeces
	memcpy(mesh_data.index_data.data(), &model.buffers[0].data[idx_buff_offset], idx_buff_size);


	//Generate tangents and add them to vector
	if (!has_tangent)
	{
		generateTangents(mesh_data);
	}

	/*
	int count = 0;
	for (int i = 0; i < mesh_data.vbuffer_data.size(); i++)
	{
		if (mesh_data.vbuffer_data[i].tangent.w == -1.0) {
			count += 1;
		}
	}
	std::cout<<count;
	*/
	return std::make_shared<Mesh>(mesh_data);

};




std::shared_ptr<Texture> AssetLoader::loadTextureFromFile(const char* filepath, ImageFormat format)//TODO add argument to edit amount if mip levels
{


	if (stbi_is_hdr(filepath))//TODO: This is kinda duplicated, somehow split this into a single function
	{
		int width, height, n_channels;
		float* pixels = stbi_loadf(filepath, &width, &height, &n_channels, STBI_rgb_alpha);
		

		n_channels = 4;//rgb aplha format

		ImageSize img_size = { (uint32_t)width, (uint32_t)height, (uint32_t)n_channels };

		if (!pixels)
		{
			std::cout << "Error loading file: " << filepath << std::endl;
			CRITICAL_ERROR_LOG("Failed to load texture file");
		}

		//Create texture
		void* data = pixels;
		ImageProperties properties = { img_size, format, 1, 1 };
		std::shared_ptr<Texture> resource = std::make_shared<Texture>(data, properties);

		stbi_image_free(pixels);

		return resource;
		
	}
	else
	{
		int width, height, n_channels;

		stbi_uc* pixels = stbi_load(filepath, &width, &height, &n_channels, STBI_rgb_alpha);

		n_channels = 4;//rgb aplha format
		ImageSize img_size{ (uint32_t)width, (uint32_t)height, (uint32_t)n_channels };

		if (!pixels)
		{
			std::cout << "Error loading file: " << filepath << std::endl;
			CRITICAL_ERROR_LOG("Failed to load texture file");
		}

		void* data = pixels;
		ImageProperties properties = { img_size, format, 1, 1 };
		std::shared_ptr<Texture> resource = std::make_shared<Texture>(data, properties);

		stbi_image_free(pixels);

		return resource;

		

	}



}


std::shared_ptr<Cubemap> AssetLoader::loadCubemapFromFiles(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz, ImageFormat format)
{

	int width, height, n_channels;

	unsigned char* texture_data[6];

	texture_data[0] = stbi_load(posx, &width, &height, &n_channels, STBI_rgb_alpha);
	texture_data[1] = stbi_load(negx, &width, &height, &n_channels, STBI_rgb_alpha);
	texture_data[2] = stbi_load(posy, &width, &height, &n_channels, STBI_rgb_alpha);
	texture_data[3] = stbi_load(negy, &width, &height, &n_channels, STBI_rgb_alpha);
	texture_data[4] = stbi_load(posz, &width, &height, &n_channels, STBI_rgb_alpha);
	texture_data[5] = stbi_load(negz, &width, &height, &n_channels, STBI_rgb_alpha);

	n_channels = 4;//rgb aplha format
	

	if (!texture_data) {//TODO: error check each file
		std::cout << "Error loading file: " << posx << std::endl;
		CRITICAL_ERROR_LOG("Failed to load texture file");
	}

	void* data = texture_data;
	ImageProperties properties = {{(uint32_t)width, (uint32_t)height, (uint32_t)n_channels }, format, 1, 6 };


	std::vector<unsigned char> sequential_data;
	uint32_t layer_size = properties.sizeInPixels();
	sequential_data.resize(layer_size * 6); //resize vector to fit all 6 images

	//Copy memory into contiguous array
	for (int i = 0; i < 6; i++) 
	{
		memcpy(&sequential_data[i*layer_size], texture_data[i], layer_size);
	}

	std::shared_ptr<Cubemap> resource = std::make_shared<Cubemap>(sequential_data.data(), properties); // add layers

	for (int i = 0; i < 6; i++) 
	{
		stbi_image_free(texture_data[i]);
	}
	return resource;
}



static uint32_t getMaxMipLevels(uint32_t width, uint32_t height, bool mipmaps)
{
	if (!mipmaps)
	{
		return 1;
	}
	else
	{
		return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
	}
}



std::shared_ptr<Cubemap> AssetLoader::loadCubemapFromEquirectMap(const char* filepath, bool mipmaps) 
{
	ImageSize img_size;
	ImageProperties properties;
	Texture equirect;
	int width, height, n_channels, cubemap_length;
	ImageFormat format;

	//Load HDR map
	if (stbi_is_hdr(filepath))//TODO: This is kinda duplicated, somehow split this into a single function
	{
		stbi_set_flip_vertically_on_load(true);
		
		float* data = stbi_loadf(filepath, &width, &height, &n_channels, STBI_rgb_alpha);
		stbi_set_flip_vertically_on_load(false);

		n_channels = 4;//rgb aplha format

		img_size = { (uint32_t)width, (uint32_t)height, (uint32_t)n_channels };
		cubemap_length = std::floor(height/2);
		if (!data)
		{
			std::cout << "Error loading file: " << filepath << std::endl;
			CRITICAL_ERROR_LOG("Failed to load texture file");
		}
		format = { VK_FORMAT_R32G32B32A32_SFLOAT };
		//Create texture
		properties = { img_size, format, 1, 1 };
		equirect = { data, properties };
		stbi_image_free(data);

		//Create empty cubemap
		format = { VK_FORMAT_R16G16B16A16_SFLOAT };
		ImageProperties cubemap_properties = { {(uint32_t)cubemap_length, (uint32_t)cubemap_length, (uint32_t)n_channels }, format, getMaxMipLevels(cubemap_length, cubemap_length, mipmaps), 6 }; //set cubemap to max size of smallest side of equirect
		StorageCubemap cubemap = { cubemap_properties }; // add layers


		//Use compute shader to convert equirectangular image to cubemap
		ComputePipeline compute_program = { "equirectToCubemapHDR" };
		DescriptorSet compute_descriptor;
		compute_descriptor.setDescriptorSetLayout(0, &compute_program);
		compute_descriptor.bindCombinedSamplerTexture(0, &equirect);
		compute_descriptor.bindStorageImage(1, &cubemap);
		compute_descriptor.buildDescriptorSet();

		CommandBuffer cmd_buff = RenderModule::getBackend()->createDisposableCmdBuffer();
		cmd_buff.calcSizeAndDispatch(compute_program, compute_descriptor, img_size);
		cmd_buff.immediateSubmit();

		if(mipmaps)
		{
			cubemap.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
			cubemap.generateMipmaps();
		}
		else
		{
			cubemap.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		}
		return std::make_shared<Cubemap>(std::move(cubemap));
	}
	else
	{
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(filepath, &width, &height, &n_channels, STBI_rgb_alpha);
		stbi_set_flip_vertically_on_load(false);

		n_channels = 4;//rgb aplha format

		img_size = { (uint32_t)width, (uint32_t)height, (uint32_t)n_channels };
		cubemap_length = std::floor(height / 2);
		if (!data)
		{
			std::cout << "Error loading file: " << filepath << std::endl;
			CRITICAL_ERROR_LOG("Failed to load texture file");
		}

		format = { VK_FORMAT_R8G8B8A8_UNORM };
		//Create texture
		properties = { img_size, format, 1, 1 };
		equirect = { data, properties };
		stbi_image_free(data);
		
		//Create empty cubemap
		ImageProperties cubemap_properties = { {(uint32_t)cubemap_length, (uint32_t)cubemap_length, (uint32_t)n_channels }, format, getMaxMipLevels(cubemap_length, cubemap_length, mipmaps), 6 }; //set cubemap to max size of smallest side of equirect
		StorageCubemap cubemap = { cubemap_properties }; // add layers


		//Use compute shader to convert equirectangular image to cubemap
		ComputePipeline compute_program = { "equirectToCubemapRGB8" };
		DescriptorSet compute_descriptor;
		compute_descriptor.setDescriptorSetLayout(0, &compute_program);
		compute_descriptor.bindCombinedSamplerTexture(0, &equirect);
		compute_descriptor.bindStorageImage(1, &cubemap);
		compute_descriptor.buildDescriptorSet();

		CommandBuffer cmd_buff = RenderModule::getBackend()->createDisposableCmdBuffer();
		cmd_buff.calcSizeAndDispatch(compute_program, compute_descriptor, img_size);
		cmd_buff.immediateSubmit();

		if (mipmaps)
		{
			cubemap.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
			cubemap.generateMipmaps();
		}
		else
		{
			cubemap.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		}

		return std::make_shared<Cubemap>(std::move(cubemap));

	}

	
	

	
	
	
}








std::shared_ptr<PBRMaterial> AssetLoader::loadPBRMaterialFromGLTF(const char* material_name, const char* gltf_filepath, std::string folder_path, const VertexDescription& vertex_desc)
{
	//Load TinyGLTF model
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err;
	std::string warn;
	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, gltf_filepath);
	if (!warn.empty()) {
		CRITICAL_ERROR_LOG(warn);
	}
	if (!err.empty()) {
		CRITICAL_ERROR_LOG(err);
	}
	if (!res) {
		CRITICAL_ERROR_LOG("Failed to load gltf file:" + std::string(gltf_filepath))
	}
	else {
		CONSOLE_LOG("Loaded material: " + std::string(material_name) + " for: " + std::string(gltf_filepath))
	}


	//List materials from gltf
	tinygltf::Material gltf_material = model.materials[0];

	//Albedo
	int albedo_idx = gltf_material.pbrMetallicRoughness.baseColorTexture.index;
	int tex_src = model.textures[albedo_idx].source;
	std::string uri = model.images[tex_src].uri;
	std::string folder = folder_path;
	std::shared_ptr<Texture> albedo = AssetLoader::loadTextureFromFile(folder.append(uri).c_str(), {VK_FORMAT_R8G8B8A8_SRGB});
	CONSOLE_LOG(" ===================== ALBEDO TEXTURE");

	//Occlusion/Metallic/roughness (ASSUMES OCCLUSION IN IN METALLIC ROUGHNESS TEXTURE. as in gltf2.0 spec)
	int roughness_metallic_idx = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
	tex_src = model.textures[roughness_metallic_idx].source;
	uri = model.images[tex_src].uri;
	folder = folder_path;
	std::shared_ptr<Texture> occlusionRoughnessMetallic = AssetLoader::loadTextureFromFile(folder.append(uri).c_str(), { VK_FORMAT_R8G8B8A8_UNORM });
	CONSOLE_LOG(" ===================== OCC TEXTURE");

	//Normal
	int normal_idx = gltf_material.normalTexture.index;
	tex_src = model.textures[normal_idx].source;
	uri = model.images[tex_src].uri;
	folder = folder_path;
	std::shared_ptr<Texture> normal = AssetLoader::loadTextureFromFile(folder.append(uri).c_str(), { VK_FORMAT_R8G8B8A8_UNORM });
	CONSOLE_LOG(" ===================== NORMAL TEXTURE");
	

	//Emmissive

	int emmissive_idx = gltf_material.emissiveTexture.index;
	tex_src = model.textures[emmissive_idx].source;
	uri = model.images[tex_src].uri;
	folder = folder_path;
	std::shared_ptr<Texture> emmissive = AssetLoader::loadTextureFromFile(folder.append(uri).c_str(), { VK_FORMAT_R8G8B8A8_SRGB });
	CONSOLE_LOG(" ===================== EMMISSIVE TEXTURE");
	/*
	MaterialData mat_data{material_name, vertex_desc};
	mat_data.addTexture(albedo, MaterialData::PBRTextures::Albedo);
	mat_data.addTexture(normal, MaterialData::PBRTextures::Normal);
	mat_data.addTexture(occlusionRoughnessMetallic, MaterialData::PBRTextures::OcclusionRoughnessMetallic);
	mat_data.addTexture(emmissive, MaterialData::PBRTextures::Emmissive);
	*/


	return std::make_shared<PBRMaterial>(material_name, vertex_desc, albedo, normal, occlusionRoughnessMetallic, emmissive);
}




std::shared_ptr<SkyboxMaterial> AssetLoader::loadSkyboxMaterialFromCubemap(const char* material_name, const std::string& filepath, const VertexDescription& vertex_desc)
{

	std::string posx = filepath;
	posx.append("_posx.jpg");
	std::string negx = filepath;
	negx.append("_negx.jpg");

	std::string posy = filepath;
	posy.append("_posy.jpg");
	std::string negy = filepath;
	negy.append("_negy.jpg");

	std::string posz = filepath;
	posz.append("_posz.jpg");
	std::string negz = filepath;
	negz.append("_negz.jpg");


	std::shared_ptr<Cubemap> cubemap = AssetLoader::loadCubemapFromFiles(posx.c_str(), negx.c_str(), posy.c_str(), negy.c_str(), posz.c_str(), negz.c_str(), { VK_FORMAT_R8G8B8A8_SRGB });
	CONSOLE_LOG(" ===================== CUBEMAP TEXTURE");
	return std::make_shared<SkyboxMaterial>(material_name, vertex_desc, cubemap);
}




std::shared_ptr<SkyboxMaterial> AssetLoader::loadSkyboxMaterialFromEquirectMap(const char* material_name, const std::string& filepath, const VertexDescription& vertex_desc)
{

	std::shared_ptr<Cubemap> cubemap = AssetLoader::loadCubemapFromEquirectMap(filepath.c_str());


	return std::make_shared<SkyboxMaterial>(material_name, vertex_desc, cubemap);

}

}