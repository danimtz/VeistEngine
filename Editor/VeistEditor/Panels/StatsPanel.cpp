#include <pch.h>

#include "StatsPanel.h"

#include "VeistEditor/EditorApp.h"//TODO: change access to EditorApp:: to another header to prevent circular dependency 

namespace VeistEditor
{

	StatsPanel::StatsPanel()
	{


	}



	void StatsPanel::onDrawPanel()
	{

		ImGui::Begin("Stats");

		std::string name = "None";
		/*if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());
		*/
		ImGui::Text("Engine stats:");
		ImGui::Text("Frame time: %f ms", (float)m_editor_app->getFrametime().getMilliseconds());

		ImGui::End();

		

	}



}