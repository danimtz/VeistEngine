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
		void onEvent(Event& event) override;


	public:

		void loadScene();

		Scene* getActiveScene() { return m_active_scene.get(); };
		Timestep& getFrametime() { return m_frametime;};


		static EditorApp& get() {return *s_Instance;};
		

	private:

		static EditorApp* s_Instance;
	
		std::unique_ptr<PanelManager> m_ui_panels;

		std::unique_ptr<Scene> m_active_scene;
		
		bool m_scene_loaded = true;
		float m_last_frame_time = 0.0f;
		Timestep m_frametime = 0.0f;


	};


};