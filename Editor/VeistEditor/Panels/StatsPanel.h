#pragma once

#include <VeistEditor.h>

#include "GUIPanel.h"

namespace VeistEditor
{


	class StatsPanel : public GUIPanel
	{
	public:

		StatsPanel();
		~StatsPanel(){};

		void onDrawPanel() override;
		void onEvent(Event& event) override {};

	private:

		//DEBUGGING
		float m_last_spike;

	};

}


