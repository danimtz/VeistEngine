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
		//ImGui::SetWindowSize();
		ImGui::Begin("Properties", (bool*)true, flags);


		if (ecs::isEntityIdValid(m_selected_entity)) //Selected entity
		{
			
			drawComponentHeader();

			ImGui::Separator();

			drawComponents();
		}

		ImGui::End();


	}



	static void drawVec3Sliders(glm::vec3& values, std::string label, float speed = 0.05f, float reset_val = 0.0f)
	{

		ImVec2 button_size = {20, 0};

		ImGui::PushID(label.c_str());
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

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
		ImGui::PopItemWidth();
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
		ImGui::PopItemWidth();
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
		auto tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (registry->hasComponent<CompType>(entity))//TODO danger if active registry not set this can crash
		{
			bool component_deleted = false;
			ImGui::PushID((void*)typeid(CompType).hash_code());

			ImVec2 content_region = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			bool opened = ImGui::TreeNodeEx((void*)typeid(CompType).hash_code(), tree_node_flags, name.c_str());
			ImGui::PopStyleVar();
			
			ImVec2 button_size = { line_height, line_height };
			ImGui::SameLine(content_region.x - line_height * 0.5f);
			if (ImGui::Button("x", button_size))
			{
				ImGui::OpenPopup("Delete component?");
			}
			
			if (ImGui::BeginPopupModal("Delete component?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Delete component?");
				ImGui::Separator();

				if (ImGui::Button("OK", ImVec2(120, 0)))
				{
					//TODO: some components from some entites should not be able to be removed
					component_deleted = true;
					registry->removeComponent<CompType>(entity);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			
			if (opened)
			{
				if (!component_deleted)
				{
					auto& component = registry->getComponent<CompType>(entity);
					drawComponentUI(component);
				}
				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}



	void EntityPropertiesPanel::drawComponents()
	{

		

		drawComponent<TransformComponent>("Transform", m_active_registry, m_selected_entity, [](auto& component)
		{
			auto table_flags = ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV;
			if (ImGui::BeginTable("transform_table", 2, table_flags))
			{
					
				for (int rows = 0; rows < 3; rows++)
				{
					ImGui::TableNextRow();
					//ImGui::TableSetupColumn("Transform titles", ImGuiTableColumnFlags);
					//ImGui::TableSetupColumn("Vec3 controls");
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




		drawComponent<CameraComponent>("CameraSettings", m_active_registry, m_selected_entity, [](auto& component)
			{
			//TODO rewrite all this one it doenst do anything its just a test
				/*ImVec2 button_size = { 20, 0 };

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.57f, 0.00f, 0.00f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.00f, 0.00f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.25f, 0.25f, 1.00f));
				if (ImGui::Button("Camera whatever", button_size))
				{
					//component. = reset_val;
				}
				float x = 10;
				ImGui::PopStyleColor(3);
				ImGui::SameLine();
				ImGui::PopStyleVar();
				ImGui::DragFloat("##TESTval", &x , 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();
				ImGui::SameLine();*/
			});





	}



	void EntityPropertiesPanel::drawComponentHeader()
	{
		auto& nametag = m_active_registry->getComponent<NametagComponent>(m_selected_entity);
		
		ImGui::Text("Name: ");

		ImGui::SameLine();
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));

		std::strncpy(buffer, nametag.nametag()->c_str(), sizeof(buffer));
		ImGui::PushItemWidth(ImGui::GetFontSize() * 8.0f);
		if (ImGui::InputText("##t", buffer, sizeof(buffer)))
		{
			nametag = std::string(buffer);
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushItemWidth(-FLT_MIN);
		if (ImGui::Button(" + Add Component"))
		{
			ImGui::OpenPopup("AddComponentsMenu");
		}

		if (ImGui::BeginPopup("AddComponentsMenu"))
		{
			if (!m_active_registry->hasComponent<CameraComponent>(m_selected_entity))
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_active_registry->emplaceComponent<CameraComponent>(m_selected_entity);
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_active_registry->hasComponent<TransformComponent>(m_selected_entity))
			{
				if (ImGui::MenuItem("Transform"))
				{
					m_active_registry->emplaceComponent<TransformComponent>(m_selected_entity);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();


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