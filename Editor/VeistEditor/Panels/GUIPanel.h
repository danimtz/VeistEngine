#pragma once



namespace VeistEditor
{ 
	class EditorApp;
	class GUIPanel
	{
	public:
		
		GUIPanel();
		virtual ~GUIPanel(){};


		virtual void renderPanel() = 0;


	protected:

		EditorApp* m_editor_app;


	private:


	};

}