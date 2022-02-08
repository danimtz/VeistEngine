#pragma once

#include "Veist/Events/Event.h"


namespace Veist
{

	

	class MouseMovedEvent : public Event
	{
	public:

		MouseMovedEvent(const float xpos, const float ypos) : m_xpos(xpos), m_ypos(ypos){}

		float getMouseX() const { return m_xpos; };
		float getMouseY() const { return m_ypos; };


		VEIST_EVENT_DECLARE_FLAGS(EventFlagInput | EventFlagMouse);
		VEIST_EVENT_DECLARE_TYPE(MouseMoved);

	private:
		
		float m_xpos;
		float m_ypos;

	};




	class MouseScrolledEvent : public Event
	{
	public:

		MouseScrolledEvent(const float xoffset, const float yoffset) : m_xoffset(xoffset), m_yoffset(yoffset) {}

		float getOffsetX() const { return m_xoffset; };
		float getOffsetY() const { return m_yoffset; };


		VEIST_EVENT_DECLARE_FLAGS(EventFlagInput | EventFlagMouse);
		VEIST_EVENT_DECLARE_TYPE(MouseScrolled);

	private:

		float m_xoffset;
		float m_yoffset;

	};




	class MouseButtonPressedEvent : public Event
	{
	public:

		MouseButtonPressedEvent(const int button) : m_mouse_button(button) {}

		int getMouseButton() const {return m_mouse_button;};


		VEIST_EVENT_DECLARE_FLAGS(EventFlagInput | EventFlagMouse);
		VEIST_EVENT_DECLARE_TYPE(MouseButtonPressed);

	private:
		
		//TODO replace with keycodes. Same for keyboard events
		int m_mouse_button;

	};



	class MouseButtonReleasedEvent : public Event
	{
	public:

		MouseButtonReleasedEvent(const int button) : m_mouse_button(button) {}

		int getMouseButton() const { return m_mouse_button; };


		VEIST_EVENT_DECLARE_FLAGS(EventFlagInput | EventFlagMouse);
		VEIST_EVENT_DECLARE_TYPE(MouseButtonPressed);

	private:

		//TODO replace with keycodes. Same for keyboard events
		int m_mouse_button;

	};



}