#include "Game.h"
#include "Graphics/Colors.h"
#include "Utilities/InputManager.h"
#include "Utilities/Time.h"
#include "JsonDocument.h"
#include "FileWatcher.h"

CGame::CGame()
{
	mySprites[static_cast<int>(ETileType::Water)].Load("Sprites/Water.dds");
	mySprites[static_cast<int>(ETileType::Plain)].Load("Sprites/Plain.dds");
	mySprites[static_cast<int>(ETileType::Forest)].Load("Sprites/Forest.dds");
	mySprites[static_cast<int>(ETileType::Mountain)].Load("Sprites/Mountain.dds");

	bool didIT = CU::FileWatcher::GetInstance().WatchFileChange(L"world.json", [&](const std::wstring&) { Init(); });
}

void CGame::Init()
{

	JsonDocument jsonDoc;
	jsonDoc.LoadFile("world.json");

	myWorld.clear();

	JsonValue data = jsonDoc["myWorldData"];
	height = data.GetSize();
	for (int y = 0; y < height; ++y)
	{
		myWorld.push_back(std::vector<ETileType>());

		JsonValue tiles = data[y];
		for (int x = 0; x < tiles.GetSize(); ++x)
		{
			width = tiles.GetSize();
			myWorld[y].push_back(static_cast<ETileType>(tiles[x].GetInt()));
		}
	}

	cameraZoom = sqrt(width * height) / 800.f;
	cameraOffset = { 400.f, 400.f };
}

void CGame::Update(float aDT)
{
	CInputManager& inputManager = CInputManager::GetInstance();

	if (inputManager.IsKeyPressed(EKeyCode::F5))
	{
		Init();
	}

	if (inputManager.IsKeyDown(EKeyCode::MouseLeft))
	{
		cameraOffset += (inputManager.GetCursorPosition() - prevMPos) / cameraZoom;
	}

	cameraZoom += (float)inputManager.GetWheelDelta() * aDT;

	prevMPos = inputManager.GetCursorPosition();


	CU::FileWatcher::GetInstance().FlushChanges();
}

void CGame::Render()
{
	int tileWidth = mySprites[0].GetDimensions().x;
	int tileHeight = mySprites[0].GetDimensions().y;
	float xScale = 1.57f ;
	tileWidth *= xScale;
	tileWidth *= cameraZoom;
	tileHeight *= cameraZoom;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int type = static_cast<int>(myWorld[y][x]);
			mySprites[type].SetPosition(tileWidth / 2 + x * tileWidth, tileHeight / 2 + y * tileHeight);
			mySprites[type].Move(cameraZoom * xScale * cameraOffset.x, cameraZoom * cameraOffset.y);
			mySprites[type].SetScale({ cameraZoom * xScale, cameraZoom * 1.f });
			mySprites[type].Render();
		}
	}
}
