#pragma once

#include <VeistEditor.h>

#include "Panels/PanelManager.h"
#include "Veist/Camera/CameraController.h"

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

		Scene* getActiveScene() { return scene; };
		Timestep& getFrametime() { return m_frametime;};


		static EditorApp& get() {return *s_Instance;};
		

	private:

		static EditorApp* s_Instance;
	
		std::unique_ptr<PanelManager> m_ui_panels;

		Scene* scene;//TEMPORARY replace with actual layer that runs the scene i.e editor or runtime
		std::shared_ptr<CameraController> m_editor_camera;
		bool m_scene_loaded = false;

		float m_last_frame_time = 0.0f;
		Timestep m_frametime = 0.0f;


	};


};