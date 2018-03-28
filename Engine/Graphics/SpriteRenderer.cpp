#include "SpriteRenderer.h"
#include <d3d11.h>
#include "Engine/DirectXFramework.h"
#include <fstream>
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"

CSpriteRenderer::CSpriteRenderer()
{
}


CSpriteRenderer::~CSpriteRenderer()
{
}

bool CSpriteRenderer::Init(CDirectXFramework * aFramework)
{
	myFramework = aFramework;

	D3D11_BUFFER_DESC desc = {0};
	desc.ByteWidth = sizeof(SSprite);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//D3D11_SUBRESOURCE_DATA subResData = { 0 };
	//subResData.pSysMem = new SSprite();
	
	HRESULT result;

	result = myFramework->GetDevice()->CreateBuffer(&desc, nullptr, &myBuffer);
	if (FAILED(result)) { return false; }

	std::ifstream vsFile;
	vsFile.open("vertexShader.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = myFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
	vsFile.close();
	if (FAILED(result)) { return false; }
	
	std::ifstream gsFile;
	gsFile.open("geometryShader.cso", std::ios::binary);
	std::string gsData = { std::istreambuf_iterator<char>(gsFile), std::istreambuf_iterator<char>() };
	result = myFramework->GetDevice()->CreateGeometryShader(gsData.data(), gsData.size(), nullptr, &myGeometryShader);
	gsFile.close();
	if (FAILED(result)) { return false; }
	
	std::ifstream psFile;
	psFile.open("pixelShader.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	result = myFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
	psFile.close();
	if (FAILED(result)) { return false; }

	//myDefaultShader.Load(EShaderType::VertexShader, "Shader/defaultShader.vfx");
	//myDefaultShader.Load(EShaderType::GeometryShader, "Shader/defaultShader.vfx");
	//myDefaultShader.Load(EShaderType::PixelShader, "Shader/defaultShader.vfx");

	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXRECT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DIMENSIONS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TRASH", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//GUID guid;
	//void* shaderData;
	//UINT shaderDataSize;
	//
	//myDefaultShader.GetVertexShader()->GetPrivateData(guid, &shaderDataSize, shaderData);

	result = myFramework->GetDevice()->CreateInputLayout(layoutDesc, 7, vsData.data(), vsData.size(), &myLayout);
	if (FAILED(result)) { return false; }

	return true;
}

void CSpriteRenderer::Render(std::vector<CSprite>& aSpritesToRender)
{
	ID3D11DeviceContext* context = myFramework->GetContext();

	static UINT stride = sizeof(SSprite);
	static UINT offset = 0;
	static HRESULT result;

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetInputLayout(myLayout);
	context->IASetVertexBuffers(0, 1, &myBuffer, &stride, &offset);

	context->VSSetShader(myVertexShader, nullptr, 0);
	context->GSSetShader(myGeometryShader, nullptr, 0);
	context->PSSetShader(myPixelShader, nullptr, 0);

	//context->VSSetShader(myDefaultShader.GetVertexShader(), nullptr, 0);
	//context->GSSetShader(myDefaultShader.GetGeometryShader(), nullptr, 0);
	//context->PSSetShader(myDefaultShader.GetPixelShader(), nullptr, 0);

	for (CSprite& sprite : aSpritesToRender)
	{
		SSprite rc;
		rc.position = sprite.GetPosition();
		rc.color = sprite.GetColor();
		rc.dimensions = { sprite.GetDimensions().x, sprite.GetDimensions().y };
		rc.rotation = sprite.GetRotation();
		rc.textureRect = { sprite.GetTextureRect().topLeft , sprite.GetTextureRect().bottomRight };
		rc.scale = { sprite.GetScale().x, sprite.GetScale().y };

		D3D11_MAPPED_SUBRESOURCE resource = { 0 };
		result = context->Map(myBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		if (FAILED(result)) { continue; }
		memcpy(resource.pData, &rc, sizeof(SSprite));
		context->Unmap(myBuffer, 0);

		context->PSSetShaderResources(0, 1, &sprite.myTexture.myTexture);

		context->Draw(1, 0);
	}
}
