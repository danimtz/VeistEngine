#include "pch.h"
#include "HierarchyPanel.h"


#include "VeistEditor/EditorApp.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"
#include "Veist/Scenes/ECS/Components/Components.h"


namespace VeistEditor
{
	
	//TODO make this faster probably
	static std::string getImGuiEntityLabel(ecs::EntityId& entity, std::string& display_name)
	{

		std::string label;

		label.append(display_name).append("##").append(std::to_string(entity));

		return label;
	}



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
		
		ImGuiTreeNodeFlags flags = ((m_selected_entity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		auto& name = m_active_registry->getComponent<NametagComponent>(entity);
		
		//Deselect entity
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			if (m_selected_entity != -1)
			{
				m_selected_entity = -1;
				EditorApp::get().processEvent(EditorEntitySelectedChangedEvent(m_selected_entity));
			}
		}

		if (ImGui::Selectable( getImGuiEntityLabel(entity, *name.nametag()).c_str(), m_selected_entity == entity))
		{
			
			if (entity != m_selected_entity)
			{
				m_selected_entity = entity;
				EditorApp::get().processEvent(EditorEntitySelectedChangedEvent(entity));
			}


		}

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