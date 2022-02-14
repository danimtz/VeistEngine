#pragma once

#include <VeistEditor.h>
#include "GUIPanel.h"
#include "Veist/Events/EditorEvents.h"

namespace VeistEditor
{

	class EntityPropertiesPanel : public GUIPanel
	{
	public:

		EntityPropertiesPanel();

		void onDrawPanel() override;
		void onEvent(Event& event) override;


	private:

		void drawComponents();
		void drawComponentHeader();

		void setSelectedEntity(EditorEntitySelectedChangedEvent& event);
		void changeScene(EditorSceneChangedEvent& event);

		ecs::EntityId m_selected_entity = -1;
	
		ecs::EntityRegistry* m_active_registry;

	};



}

