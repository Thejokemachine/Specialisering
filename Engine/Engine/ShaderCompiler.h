#pragma once
#include "Base.h"
#include <map>;

class CShader;

enum class EShaderType
{
	VertexShader,
	GeometryShader,
	PixelShader
};

class CShaderCompiler : public CBase
{
public:
	CShaderCompiler();
	~CShaderCompiler();

	void CompileShader(const std::string& aFileName, const std::string& aIncludePath, const std::string& aEntryPoint, EShaderType aShaderType, CShader& aShaderToLoadInto);

private:

};

