#include <windows.h>
#include "Engine/WindowHandler.h"
#include "Engine/GraphicsEngine.h"
#include "Game/Game.h"
#include "Utilities/Time.h"
#include "Utilities/InputManager.h"
#include "Utilities/DebugLog.h"

int WINAPI wWinMain(_In_ HINSTANCE hIntance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	hIntance; hPrevInstance; lpCmdLine, nShowCmd;
	
	CGraphicsEngine graphicsEngine;

	CWindowHandler::SWindowData windowData;
	windowData.width = 800;
	windowData.height = 800;
	graphicsEngine.GetDesktopResolution(windowData.x, windowData.y);
	windowData.x /= 2;
	windowData.y /= 2;
	windowData.x -= windowData.width / 2;
	windowData.y -= windowData.height / 2;
	windowData.windowed = true;
	windowData.clearColor = { 0.2f, 0.2f, 0.2f, 1.f };

	bool shouldRun = graphicsEngine.Init(windowData);

	CGame game;
	CTime& timer = CTime::GetInstance();

	if (shouldRun)
	{
		FILE* file;

		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&file, "CON", "w", stdout);

		DL_WRITELOG("engine", "Console Initiated.");

		game.Init();
		timer.Init();
		CInputManager::GetInstance().Init(graphicsEngine.GetClientWindow());
	}

	MSG windowsMessage = { };

	while (shouldRun)
	{
		CInputManager::GetInstance().OncePerFrameUpdate();

		while (PeekMessage(&windowsMessage, 0, 0, 0, PM_REMOVE))
		{
			CInputManager::GetInstance().Update(windowsMessage);

			if (windowsMessage.message == WM_QUIT)
			{
				shouldRun = false;
			}

			TranslateMessage(&windowsMessage);
			DispatchMessage(&windowsMessage);
		}

		graphicsEngine.BeginFrame();

		timer.Update();
		game.Update(timer.GetDeltaTime());
		game.Render();

		graphicsEngine.RenderFrame();

		graphicsEngine.EndFrame();
	}

	return 0;
}