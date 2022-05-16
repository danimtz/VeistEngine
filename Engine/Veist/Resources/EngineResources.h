
#include "Veist/Material/Material.h"


namespace Veist
{

//TODO maybe move this to the editor, not engine
	class EngineResources
	{
	public:
		enum MaterialTypes
		{
			PBRMaterial,
			SkyboxMaterial,
			DeferredGBufferMaterial,
			DeferredLightingMaterial,

			BillboardMaterial,

			EditorTargetSelectMaterial,
			WireframeMaterial,
			ShadowMapMaterial,

			MaxMaterialTypes
		};

		enum Materials
		{
			EditorBillboardIcons,

			MaxCompleteMaterials
		};


		enum Meshes
		{
			BillboardMesh,

			MaxPreBuiltMeshes
		};


		//TODO add compute programs here too?

		EngineResources();
		~EngineResources() 
		{
		};


		MaterialType* getMaterialType(MaterialTypes type);
		Material* getMaterial(Materials type);
		Mesh* getMesh(Meshes type);

	private:

		std::vector<MaterialSettings> m_material_settings;

		std::unique_ptr< std::vector<MaterialType> > m_material_types;

		//TODO i dont like how materials is used here. probably replace once asset system implemented. and maybe it should be a map not a vector
		std::vector<std::unique_ptr<Material>> m_materials;
		std::vector<std::unique_ptr<Mesh> > m_meshes; //Maybe replace unique pointer for vector of unique pointers
		//std::unique_ptr<std::vector<ImageBase>> m_images; //TODO: this should be Asset<ImageBase>[] when asset system implemented

		//std::unique_ptr<Mesh[]> m_meshes;
	};

}