#pragma once
#include "Base.h"
#include <vector>

class CSprite;

class CScene : CBase
{
public:
	CScene();
	~CScene();

	bool Init();
	void SetSpriteBufferSize(unsigned int aSize);
	bool AddSprite(CSprite* aSprite);
	std::vector<CSprite>& GetSpriteBuffer();
	void Clear();

	//void Cull();

private:
	std::vector<CSprite> mySpriteBuffer;
	unsigned int myAvailableIndex;
};

