#pragma once
#include <defines.hpp>
#include <objecttemplates/singleton.hpp>

BEGIN_NAMESPACE

namespace Input
{
	class InputManager SINGLETON(InputManager)
	{
	private:
		SINGLETON_CONSTRUCTOR(InputManager)

	};
}

END_NAMESPACE