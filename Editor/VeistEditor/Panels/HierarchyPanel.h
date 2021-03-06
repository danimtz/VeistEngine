#pragma once

#include <VeistEditor.h>
#include "GUIPanel.h"
#include "Veist/Events/EditorEvents.h"

namespace VeistEditor
{

	class HierarchyPanel : public GUIPanel
	{
	public:

		HierarchyPanel();

		void onDrawPanel() override;
		void onEvent(Event& event) override;


	private:
		
		void drawEntity(ecs::EntityId& entity);

		void changeScene(EditorSceneChangedEvent& event);

		ecs::EntityId m_selected_entity = -1;

		ecs::EntityRegistry* m_active_registry;
	
	};



}

