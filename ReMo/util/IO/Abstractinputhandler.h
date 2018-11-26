#ifndef __REMO_INPUTHANDLER_ABSTRACTINPUTHANDLER_H__
#define __REMO_INPUTHANDLER_ABSTRACTINPUTHANDLER_H__

#include <cstdint>
#include <string>

namespace remo
{
	namespace inputHandler
	{
		enum class MouseInputButton : std::uint8_t
		{
			MIB_BUTTON_0 = 0,
			MIB_BUTTON_1 = 1,
			MIB_BUTTON_2 = 2,
			MIB_WHEEL = 3,
			MIB_UNKNOWN = 4
		};

		enum class MouseInputState : std::uint8_t
		{
			MIS_BUTTON_DOWN = 0,
			MIS_BUTTON_UP = 1,
			MIS_BUTTON_DBL_CLICK = 2,
			MIS_DUMMY = 3
		};

		enum class KeyInputState : std::uint8_t
		{
			KIS_KEY_DOWN = 0,
			KIS_KEY_UP = 1,
			KIS_KEY_PRESS = 2,
			KIS_KEY_UNKNOWN = 3
		};

		class AbstractInputHandler
		{
			public:
				virtual void handleMouseInput(
					int screenX,
					int screenY,
					MouseInputButton btn,
					MouseInputState btnState) = 0;

				virtual void handleKeyInput(
					const std::string & key,
					KeyInputState keyState) = 0;
		};
	}
}

#endif
