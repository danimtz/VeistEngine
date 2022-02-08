#pragma once
#include "Event.h"

namespace Veist
{

	class KeyboardEvent : public Event
	{
	public:

		int getKeycode() const { return m_keycode;};

		VEIST_EVENT_DECLARE_FLAGS(EventFlagInput | EventFlagKeyboard);

	protected:

		KeyboardEvent(const int keycode) : m_keycode(keycode) {}

		int m_keycode;

	};




	class KeyTypedEvent : public KeyboardEvent
	{
	public:

		KeyTypedEvent(const int keycode) : KeyboardEvent(keycode) {};

		VEIST_EVENT_DECLARE_TYPE(KeyTyped);

	};

	class KeyPressedEvent : public KeyboardEvent
	{
	public:

		KeyPressedEvent(const int keycode, const uint16_t repeat_count) : KeyboardEvent(keycode), m_repeat_count(repeat_count) {};

		uint16_t getRepeatCount() const {return m_repeat_count;};

		VEIST_EVENT_DECLARE_TYPE(KeyPressed);
	
	private:

		uint16_t m_repeat_count;

	};

	class KeyReleasedEvent : public KeyboardEvent
	{
	public:

		KeyReleasedEvent(const int keycode) : KeyboardEvent(keycode) {};

		VEIST_EVENT_DECLARE_TYPE(KeyReleased);

	};


}