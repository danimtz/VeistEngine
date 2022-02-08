#pragma once

#include "Veist/Events/Event.h"
#include "Veist/Scenes/Scene.h"

namespace Veist
{


	class EditorSceneChangedEvent : public Event
	{
	public:
		EditorSceneChangedEvent(Scene* new_scene) : m_new_scene(new_scene) {};

		Scene* getNewScene() {return m_new_scene;};
	
		VEIST_EVENT_DECLARE_FLAGS(EventFlagExternal);
		VEIST_EVENT_DECLARE_TYPE(EditorSceneChanged);

	private:

		Scene* m_new_scene;
	};


}