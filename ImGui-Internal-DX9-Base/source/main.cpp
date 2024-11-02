#include "includes.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);
LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);
const char* ui::windowName = "Hack";

bool ui::InitializeWindowClass(const char* windowClassName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	if (!RegisterClassEx(&windowClass))
		return false;

	return true;
}

bool ui::InitializeWindow(const char* windowClassName) noexcept
{
	window = CreateWindow
	(
		windowClass.lpszClassName,
		windowName,
		WS_POPUP,
		0,
		0,
		100,
		100,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);

	if (!window)
	{
		OutputDebugString("Failed to create window.");
		return false;
	}

	OutputDebugString("created successfully.");
	return true;
}

void ui::DestroyWindow() noexcept
{
	if (window)
		DestroyWindow(window);
}

void ui::DestroyDirectX() noexcept
{
	if (device)
	{
		device->Release();
		device = NULL;
	}

	if (d3d9)
	{
		d3d9->Release();
		d3d9 = NULL;
	}
}

void ui::DestroyWindowClass() noexcept
{
	UnregisterClass
	(
		windowClass.lpszClassName,
		windowClass.hInstance
	);
}

void ui::Initialize()
{
	if (!InitializeWindowClass("hackClass001"))
	{

	}

	if (!InitializeWindow("Hack Window"))
	{

	}

	if (!InitializeDirectX())
	{

	}
	else
	{
		OutputDebugString("Failed to create window.");
	}

	DestroyWindow();
	DestroyWindowClass();
}

void ui::InitializeMenu(LPDIRECT3DDEVICE9 device) noexcept
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{};
	device->GetCreationParameters(&params);
	window = params.hFocusWindow;

	originalWindowProcess = reinterpret_cast<WNDPROC>
		(
			SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess))
			);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;
}

void ui::Destroy() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr
	(
		window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(originalWindowProcess)
	);

	DestroyDirectX();
}

void ui::Render() noexcept
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(550, 350));
	ImGui::Begin("Imgui Internal DX9 base | Queecy", nullptr, ImGuiWindowFlags_NoResize);
	ImGui::Text("Imgui Internal DX9 base | Queecy");

	if (ImGui::Button("Github")) {
		ShellExecute(0, 0, "https://github.com/Queecy", 0, 0, SW_SHOW);
	}
	ImGui::SameLine();
	if (ImGui::Button("Telegram")) {
		ShellExecute(0, 0, "https://t.me/notQueecy", 0, 0, SW_SHOW);
	}

	static bool checkbox_state = false;
	ImGui::Checkbox("Checkbox", &checkbox_state);

	static char buf[256] = "";
	ImGui::InputText("input text", buf, IM_ARRAYSIZE(buf));

	static int slider_value = 0;
	ImGui::SliderInt("Slide", &slider_value, 0, 100);

	static ImVec4 color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGui::ColorEdit3("select color", (float*)&color);

	const char* items[] = { "Option1", "Option2", "Option3" };
	static int item_current = 0;
	ImGui::Combo("Select option", &item_current, items, IM_ARRAYSIZE(items));

	ImGui::Separator();
	static float float_value = 0.0f;
	ImGui::SliderFloat("Float Slider", &float_value, 0.0f, 1.0f);

	ImGui::ProgressBar(0.5f, ImVec2(0.0f, 0.0f), "50%"); // Optional text
	ImGui::Text("Tooltip");
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("tooltip text");
	}

	ImGui::End();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParam, LPARAM longParam)
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		ui::open = !ui::open;
		OutputDebugString("Toggled ImGui window.");
	}

	if (ui::open && ImGui_ImplWin32_WndProcHandler(window, message, wideParam, longParam))
		return 1L;

	return CallWindowProc
	(
		ui::originalWindowProcess,
		window,
		message,
		wideParam,
		longParam
	);
}

bool ui::InitializeDirectX() noexcept
{
	const auto handle = GetModuleHandle("d3d9.dll");
	if (!handle)
		return false;

	using CreateFN = LPDIRECT3D9(__stdcall*)(UINT);
	const auto create = reinterpret_cast<CreateFN>(GetProcAddress
	(
		handle,
		"Direct3DCreate9"
	));

	if (!create)
		return false;

	d3d9 = create(D3D_SDK_VERSION);
	if (!d3d9)
		return false;

	D3DPRESENT_PARAMETERS params = {};

	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice
	(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_NULLREF,
		window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
		&params,
		&device
	) < 0) return false;

	return true;
}

void hooks::Initialize()
{
	if (MH_Initialize())
		throw std::runtime_error("Unable to initialize minhook");

	if (MH_CreateHook(
		VirtualFunction(ui::device, 42),
		&EndScene,
		reinterpret_cast<void**>(&EndSceneOriginal)
	))
		throw std::runtime_error("Unable to hook EndScene()");

	if (MH_CreateHook(
		VirtualFunction(ui::device, 16),
		&Reset,
		reinterpret_cast<void**>(&ResetOriginal)
	))
		throw std::runtime_error("Unable to hook Reset()");

	if (MH_EnableHook(MH_ALL_HOOKS))
		throw std::runtime_error("Unable to enable hooks");

	ui::DestroyDirectX();
}

void hooks::CleanUP()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

long __stdcall hooks::EndScene(IDirect3DDevice9* device) noexcept
{
	static const auto returnAddress = _ReturnAddress();

	OutputDebugString("EndScene called.");

	const auto result = EndSceneOriginal(device, device);
	if (_ReturnAddress() == returnAddress)
		return result;

	if (!ui::setup)
	{
		ui::InitializeMenu(device);
		OutputDebugString("InitializeMenu called.");
	}

	if (ui::open)
		ui::Render();

	return result;
}

HRESULT __stdcall hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;

}
