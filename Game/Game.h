#pragma once
#include "Graphics/Sprite.h"
#include "Engine/Shader.h"

class CGame
{
public:
	CGame();
	~CGame() = default;

	void Init();
	void Update(float aDT);
	void Render();

private:

	enum class ETileType
	{
		Water,
		Plain,
		Forest,
		Mountain,
		Count
	};

	std::vector<std::vector<ETileType>> myWorld;
	int width;
	int height;
	CSprite mySprites[static_cast<int>(ETileType::Count)];

	CVector2f prevMPos;
	CVector2f cameraOffset;
	float cameraZoom;
};

