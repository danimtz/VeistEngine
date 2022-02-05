#pragma once

#include <VeistEditor.h>

#include "Panels/PanelManager.h"


namespace VeistEditor
{



	class EditorApp : public Application
	{
	public:

		EditorApp();

		~EditorApp() {}

		void initClient() override;
		void runClient() override;
		void shutdownClient() override;


	public:

		void loadScene();

		Scene* getActiveScene() { return scene; };

		static EditorApp& get() {return *s_Instance;};
		

	private:

		static EditorApp* s_Instance;
		
		std::unique_ptr<PanelManager> m_ui_panels;
		Scene* scene;//TEMPORARY replace with actual layer that runs the scene i.e editor or runtime

	};


};