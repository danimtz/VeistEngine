#include "pch.h"
#include "HierarchyPanel.h"


#include "VeistEditor/EditorApp.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"
#include "Veist/Scenes/ECS/Components/Components.h"

namespace VeistEditor
{
	

	HierarchyPanel::HierarchyPanel() 
	{
	

		m_active_registry = EditorApp::get().getActiveScene()->ecsRegistry();


	}



	void HierarchyPanel::onDrawPanel()
	{
		ImGui::Begin("Hierarchy", nullptr);


		for (auto& entity : *m_active_registry->entityList())
		{
			drawEntity(entity);
		}


		ImGui::End();



		ImGui::Begin("Properties");
		if (false) //Selected entity
		{
			//drawComponents(m_selected_entity);
		}

		ImGui::End();


	}




	void HierarchyPanel::drawEntity(ecs::EntityId& entity)
	{
		
		//ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		auto& name = m_active_registry->getComponent<NametagComponent>(entity);

		
		bool opened = ImGui::TreeNodeEx(name.nametag()->c_str(), 0);
		

		if (opened)
		{
			/*ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, "TEST");
			if (opened)
				ImGui::TreePop();*/
			ImGui::TreePop();
		}

	}


	void HierarchyPanel::drawComponents(ecs::EntityId& entity)
	{

	}


	void HierarchyPanel::onEvent(Event& event)
	{
		EventHandler handler(event);

		handler.handleEvent<EditorSceneChangedEvent>(VEIST_EVENT_BIND_FUNCTION(HierarchyPanel::changeScene));
	}



	void HierarchyPanel::changeScene(EditorSceneChangedEvent& event)
	{

		m_active_registry = event.getNewScene()->ecsRegistry();
		
	}

}