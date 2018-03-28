#pragma once
#include "Base.h"
#include "Math/Vector2.h"
#include "Math/Rectangle.h"
#include <vector>
#include "ShaderWrappers.h"
#include "../Engine/Shader.h"

struct ID3D11PixelShader;
struct ID3D11GeometryShader;
struct ID3D11VertexShader;
struct ID3D11Buffer;
struct ID3D11InputLayout;

class CDirectXFramework;
class CSprite;

class CSpriteRenderer : CBase
{
public:
	CSpriteRenderer();
	~CSpriteRenderer();

	bool Init(CDirectXFramework* aFramework);
	void Render(std::vector<CSprite>& aSpritesToRender);

private:

	//CShader myDefaultShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11GeometryShader* myGeometryShader;
	ID3D11VertexShader* myVertexShader;
	ID3D11Buffer* myBuffer;
	ID3D11InputLayout* myLayout;

	CDirectXFramework* myFramework;
};

