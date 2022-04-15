
#include "Veist/Material/Material.h"


namespace Veist
{


	class EngineResources
	{
	public:
		enum MaterialTypes
		{
			PBRMaterial,
			SkyboxMaterial,
			DeferredGBufferMaterial,
			DeferredLightingMaterial,
			
			MaxMaterialTypes
		};


        /*enum ShaderNames
        {
            //EquirecConvolutionComp,
            //CubemapConvolutionComp,
            
			PBRShaderVert,
			PBRShaderFrag,

            MaxShaders
        };*/

		EngineResources();
		~EngineResources(){};


		MaterialType* getMaterialType(MaterialTypes type);

	private:


		std::unique_ptr< std::vector<MaterialType> > m_material_types;
		//std::unique_ptr<Material[]> m_materials;
		std::unique_ptr<std::vector<ImageBase>> m_images; //TODO: this should be Asset<ImageBase>[] when asset system implemented

		//std::unique_ptr<Mesh[]> m_meshes;
	};

}


