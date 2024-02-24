#include "Console.h"

namespace engine::general
{
	void initConsole()
	{
		AllocConsole();
		FILE* dummy;

		freopen_s(&dummy, "CONIN$", "r", stdin);
		freopen_s(&dummy, "CONOUT$", "w", stdout);
		freopen_s(&dummy, "CONOUT$", "w", stderr);

	}
}
