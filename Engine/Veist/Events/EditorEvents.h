#pragma once

#include "Veist/Events/Event.h"
#include "Veist/Scenes/Scene.h"
#include "Veist/Scenes/ECS/ECSTypes.h"
namespace Veist
{


	class EditorSceneChangedEvent : public Event
	{
	public:
		EditorSceneChangedEvent(Scene* new_scene) : m_new_scene(new_scene) {};

		Scene* getNewScene() {return m_new_scene;};
	
		VEIST_EVENT_DECLARE_FLAGS(EventFlagEditor);
		VEIST_EVENT_DECLARE_TYPE(EditorSceneChanged);

	private:

		Scene* m_new_scene;
	};



	class EditorEntitySelectedChangedEvent : public Event
	{
	public:

		EditorEntitySelectedChangedEvent(ecs::EntityId m_entity) : m_entity(m_entity) {};

		ecs::EntityId getEntity() { return m_entity; };

		VEIST_EVENT_DECLARE_FLAGS(EventFlagEditor);
		VEIST_EVENT_DECLARE_TYPE(EditorEntitySelectedChanged);

	private:

		ecs::EntityId m_entity;
	};

}