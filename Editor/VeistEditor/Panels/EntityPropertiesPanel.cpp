#include "pch.h"
#include "EntityPropertiesPanel.h"


#include "VeistEditor/EditorApp.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"
#include "Veist/Scenes/ECS/Components/Components.h"


namespace VeistEditor
{
	
	//TODO make this faster probably THIS IS REPEATED FROM HIERARCHY PANEL
	static std::string getImGuiEntityLabel(ecs::EntityId& entity, std::string& display_name)
	{

		std::string label;

		label.append(display_name).append("##").append(std::to_string(entity));

		return label;
	}


	EntityPropertiesPanel::EntityPropertiesPanel()
	{

		m_active_registry = EditorApp::get().getActiveScene()->ecsRegistry();

	}



	void EntityPropertiesPanel::onDrawPanel()
	{
		
		auto flags = ImGuiWindowFlags_NoCollapse;
		ImGui::Begin("Properties", (bool*)true, flags);
		if (ecs::isEntityIdValid(m_selected_entity)) //Selected entity
		{
			drawComponents();
		}

		ImGui::End();


	}



	static void drawVec3Sliders(glm::vec3& values, std::string label, float speed = 0.1f, float reset_val = 0.0f)
	{

		ImVec2 button_size = {20, 0};

		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(40);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.57f, 0.00f, 0.00f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.00f, 0.00f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.25f, 0.25f, 1.00f));
		if (ImGui::Button("X", button_size))
		{ 
			values.x = reset_val;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::PopStyleVar();
		ImGui::DragFloat("##Xval", &values.x, speed, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine();


		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.57f, 0.00f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.00f, 0.80f, 0.00f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.9f, 0.25f, 1.00f));
		if (ImGui::Button("Y", button_size))
		{
			values.y = reset_val;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::PopStyleVar();
		ImGui::DragFloat("##Yval", &values.y, speed, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine();
		

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.22f, 0.60f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.00f, 0.32f, 0.82f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.13f, 0.52f, 1.00f, 1.00f));
		if (ImGui::Button("Z", button_size))
		{
			values.z = reset_val;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::PopStyleVar();
		ImGui::DragFloat("##Zval", &values.z, speed, 0.0f, 0.0f, "%.2f");
		
		
		ImGui::PopItemWidth();
		ImGui::PopID();

	}



	template<typename CompType, typename F>
	static void drawComponent(std::string name, ecs::EntityRegistry* registry, ecs::EntityId& entity, F drawComponentUI)
	{
		auto tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (registry->hasComponent<CompType>(entity))//TODO danger if active registry not set this can crash
		{
			
			bool opened = ImGui::TreeNodeEx((void*)typeid(CompType).hash_code(), tree_node_flags, name.c_str());
			auto& component = registry->getComponent<CompType>(entity);
			if (opened)
			{
				drawComponentUI(component);
				ImGui::TreePop();
			}
		}
	}



	void EntityPropertiesPanel::drawComponents()
	{

		drawComponent<NametagComponent>("Nametag", m_active_registry, m_selected_entity, [](auto& component)
			{
				auto table_flags = ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV;
				if (ImGui::BeginTable("nametag_table", 2, table_flags))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Variable name: ");
					ImGui::TableNextColumn();
					char buffer[256];
					memset(buffer, 0, sizeof(buffer));
					std::strncpy(buffer, component.nametag()->c_str(), sizeof(buffer));
					ImGui::SetNextItemWidth(150.0f);
					if (ImGui::InputText("##t", buffer, sizeof(buffer)))
					{
						component = std::string(buffer);
					}
					ImGui::EndTable();
				}
			});

		drawComponent<TransformComponent>("Transform", m_active_registry, m_selected_entity, [](auto& component)
			{
				auto table_flags = ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV;
				if (ImGui::BeginTable("transform_table", 2, table_flags))
				{
					
					for (int rows = 0; rows < 3; rows++)
					{
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						switch (rows)
						{
							case 0:
								ImGui::Text("Translation:");
								ImGui::TableNextColumn();
								drawVec3Sliders(component.translation(), "##trans");
								break;
							case 1:
								ImGui::Text("Rotation:");
								ImGui::TableNextColumn();
								drawVec3Sliders(component.rotation(), "##rot", 0.5f);
								break;
							case 2:
								ImGui::Text("Scale:");
								ImGui::TableNextColumn();
								drawVec3Sliders(component.scale(), "##scale", 0.1f, 1.0f);
								break;
						}
					}
					ImGui::EndTable();
					
				}

			});

	}




	void EntityPropertiesPanel::onEvent(Event& event)
	{
		EventHandler handler(event);

		handler.handleEvent<EditorSceneChangedEvent>(VEIST_EVENT_BIND_FUNCTION(EntityPropertiesPanel::changeScene));
		handler.handleEvent<EditorEntitySelectedChangedEvent>(VEIST_EVENT_BIND_FUNCTION(EntityPropertiesPanel::setSelectedEntity));
	}



	void EntityPropertiesPanel::changeScene(EditorSceneChangedEvent& event)
	{

		m_active_registry = event.getNewScene()->ecsRegistry();

	}


	void EntityPropertiesPanel::setSelectedEntity(EditorEntitySelectedChangedEvent& event)
	{
		m_selected_entity = event.getEntity();
	}



}