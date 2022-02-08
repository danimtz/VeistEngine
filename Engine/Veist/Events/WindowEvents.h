#pragma once
#include "Event.h"




namespace Veist
{

	class WindowCloseEvent : public Event
	{
	public:

		WindowCloseEvent() = default;

		VEIST_EVENT_DECLARE_FLAGS(EventFlagApp);
		VEIST_EVENT_DECLARE_TYPE(WindowClose);
		
	};


	class WindowResizeEvent : public Event
	{
	public:

		WindowResizeEvent(uint32_t width, uint32_t height) : m_width(width), m_height(height) {}

		uint32_t getWidth() const {return m_width;};
		uint32_t getHeight() const {return m_height;};
		VEIST_EVENT_DECLARE_FLAGS(EventFlagApp);
		VEIST_EVENT_DECLARE_TYPE(WindowResize);

	private:
		uint32_t m_width;
		uint32_t m_height;

	};

}