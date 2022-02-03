#include <pch.h>

#include "PanelManager.h"


namespace VeistEditor
{
	
	PanelManager::PanelManager()
	{

	}


	void PanelManager::onUpdate()
	{
		
		for (auto& gui_panel : m_gui_panels)
		{
			gui_panel.get()->renderPanel();
		}
		

	}


}