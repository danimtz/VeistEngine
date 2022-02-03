#pragma once



namespace VeistEditor
{ 
	class GUIPanel
	{
	public:
		
		GUIPanel(){};
		virtual ~GUIPanel(){};


		virtual void renderPanel() = 0;


	protected:



	private:


	};

}