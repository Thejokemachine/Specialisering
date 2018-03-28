#include "InputManager.h"

CInputManager::CInputManager()
{
}


CInputManager::~CInputManager()
{
}

CInputManager & CInputManager::GetInstance()
{
	static CInputManager instance;

	return instance;
}

void CInputManager::Init(HWND aClientWindow)
{
	myHWND = aClientWindow;
	CreateKeyStates(myKeyStates);
	myPreviousKeyStates = myKeyStates;
}

void CInputManager::Update(const MSG& aWindowsMessage)
{
	UpdateKeys(aWindowsMessage);
	UpdateMouse(aWindowsMessage);
}

void CInputManager::UpdateKeys(const MSG & aWindowsMessage)
{
	switch (aWindowsMessage.message)
	{
	case WM_KEYDOWN:
	{
		EKeyCode key = static_cast<EKeyCode>(aWindowsMessage.wParam);

		if (myKeyStates.find(key) != myKeyStates.end())
		{
			myKeyStates[key] = EKeyState::Down;
		}

		break;
	}
	case WM_KEYUP:
	{
		EKeyCode key = static_cast<EKeyCode>(aWindowsMessage.wParam);

		if (myKeyStates.find(key) != myKeyStates.end())
		{
			myKeyStates[key] = EKeyState::Up;
		}

		break;
	}
	case WM_LBUTTONDOWN:
	{
		EKeyCode key = EKeyCode::MouseLeft;

		myKeyStates[key] = EKeyState::Down;

		break;
	}
	case WM_LBUTTONUP:
	{
		EKeyCode key = EKeyCode::MouseLeft;

		myKeyStates[key] = EKeyState::Up;

		break;
	}
	case WM_MBUTTONDOWN:
	{
		EKeyCode key = EKeyCode::MouseMiddle;

		myKeyStates[key] = EKeyState::Down;

		break;
	}
	case WM_MBUTTONUP:
	{
		EKeyCode key = EKeyCode::MouseMiddle;

		myKeyStates[key] = EKeyState::Up;

		break;
	}
	case WM_RBUTTONDOWN:
	{
		EKeyCode key = EKeyCode::MouseRight;

		myKeyStates[key] = EKeyState::Down;

		break;
	}
	case WM_RBUTTONUP:
	{
		EKeyCode key = EKeyCode::MouseRight;

		myKeyStates[key] = EKeyState::Up;

		break;
	}
	}
}

void CInputManager::UpdateMouse(const MSG & aWindowsMessage)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(myHWND, &point);

	myCursorPosition.x = point.x;
	myCursorPosition.y = point.y;

	if (aWindowsMessage.message == WM_MOUSEWHEEL)
	{
		myWheelDelta = GET_WHEEL_DELTA_WPARAM(aWindowsMessage.wParam);
		myWheelDelta /= WHEEL_DELTA;
	}
}

void CInputManager::OncePerFrameUpdate()
{
	myPreviousKeyStates = myKeyStates;
	myWheelDelta = 0.0f;
}

bool CInputManager::IsKeyPressed(EKeyCode aKey)
{
	return myKeyStates[aKey] == EKeyState::Down && myPreviousKeyStates[aKey] == EKeyState::Up;
}

bool CInputManager::IsKeyDown(EKeyCode aKey)
{
	return myKeyStates[aKey] == EKeyState::Down;
}

CVector2f CInputManager::GetCursorPosition()
{
	return myCursorPosition;
}

int CInputManager::GetWheelDelta()
{
	return myWheelDelta;
}
