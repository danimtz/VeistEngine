#pragma once
#include <functional>



namespace Veist
{

	enum EventFlags
	{
		None = 0,
		EventFlagInput = 0x01,
		EventFlagMouse = 0x02,
		EventFlagKeyboard = 0x04,
		EventFlagApp = 0x08,
		EventFlagEditor = 0x16
	};

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		EditorSceneChanged, EditorEntitySelectedChanged/*Maybe the editor event shouldnt be here*/
	};

	#define VEIST_EVENT_DECLARE_FLAGS(flags) virtual int getEventFlags() const {return flags;};
	#define VEIST_EVENT_DECLARE_TYPE(type) static EventType getStaticType() {return EventType::type; }\
										virtual EventType getEventType() const override {return getStaticType();}\
										virtual const char* getName() const override { return #type; };


	#define VEIST_EVENT_BIND_FUNCTION(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
	#define VEIST_EVENT_BIND_STATIC_FUNCTION(fn) [](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }

	class Event
	{
	public:

		virtual EventType getEventType() const = 0;
		virtual int getEventFlags() const = 0;
		virtual const char* getName() const = 0;
		
		bool m_handled = false;
	};



	class EventHandler
	{
	public:
		
		EventHandler(Event& event) : m_event(event) {};

		template<typename T, typename F>
		bool handleEvent(const F& func)
		{
			if (m_event.getEventType() == T::getStaticType())
			{
				func(static_cast<T&>(m_event)); //this could be bool and set handled flag to true if succeeds
				m_event.m_handled = true;
				return true;
			}
			return false;
		}


	private:
		
		Event& m_event;

	};


	

}

