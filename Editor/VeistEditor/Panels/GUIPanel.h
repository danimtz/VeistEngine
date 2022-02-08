#pragma once

#include "Veist/Events/Event.h"

namespace VeistEditor
{ 
	class EditorApp;
	class GUIPanel
	{
	public:
		
		GUIPanel();
		virtual ~GUIPanel(){};


		virtual void onDrawPanel() = 0;
		virtual void onEvent(Veist::Event& event) = 0;

	protected:

		EditorApp* m_editor_app;


	private:


	};

}