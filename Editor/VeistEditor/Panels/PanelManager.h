#pragma once
#include <VeistEditor.h>

#include "GUIPanel.h"

namespace VeistEditor
{
	class EditorApp;
	class PanelManager
	{
	public:

		PanelManager();

		void onUpdate();

		void onEvent(Event& event);

		template<typename PanelType>
		void addPanel()
		{
			m_gui_panels.emplace_back(std::make_unique<PanelType>());
		
		
		};

		

	private:

		std::vector<std::unique_ptr<GUIPanel>> m_gui_panels;

		EditorApp* m_editor_app;


	};


}
