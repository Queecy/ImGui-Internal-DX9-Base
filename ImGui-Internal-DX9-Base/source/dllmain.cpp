#include "includes.h"
#define WIN32_LEAN_AND_MEAN

void Configuration(const HMODULE instance)
{
	try
	{
		ui::Initialize();
		hooks::Initialize();
	}
	catch (const std::exception& error)
	{
		goto UNLOAD;
	}
	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

UNLOAD:
	hooks::CleanUP();
	ui::Destroy();
	FreeLibraryAndExitThread(instance, 0);
}

BOOL WINAPI DllMain
(
	const HMODULE instance,
	const std::uintptr_t reason,
	const void* reserved
)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(instance);
		OutputDebugString("DLL_PROCESS_ATTACH");

		const auto thread = CreateThread
		(
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(Configuration),
			instance,
			0,
			nullptr
		);
	}
	return true;
}