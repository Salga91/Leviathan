#ifndef LEVIATHAN_LIGHTSHADER
#define LEVIATHAN_LIGHTSHADER
// ------------------------------------ //
#ifndef LEVIATHAN_DEFINE
#include "Define.h"
#endif
// ------------------------------------ //
// ---- includes ---- //
#include "FileSystem.h"
#include "Window.h"

#include <d3dx11async.h>

#include "ShaderDataTypes.h"


namespace Leviathan{

	class LightShader : EngineComponent{
	public:
		DLLEXPORT LightShader();
		DLLEXPORT ~LightShader();
		DLLEXPORT bool Init(ID3D11Device* device, Window* wind);
		DLLEXPORT void Release();
		DLLEXPORT bool Render(ID3D11DeviceContext* devcont,int indexcount, D3DXMATRIX worldmatrix, D3DXMATRIX viewmatrix, D3DXMATRIX projectionmatrix, ID3D11ShaderResourceView* texture,
									Float3 lightDirection, Float4 ambientColor, Float4 diffuseColor, Float3 cameraPosition, Float4 specularColor, float specularPower);

	private:
		bool Inited;
		//struct MatrixBufferType
		//{
		//	D3DXMATRIX world;
		//	D3DXMATRIX view;
		//	D3DXMATRIX projection;
		//};
		//struct CameraBufferType
		//{
		//	D3DXVECTOR3 cameraPosition;
		//	float padding;
		//};

		//struct LightBufferType
		//{
		//	D3DXVECTOR4 ambientColor;
		//	D3DXVECTOR4 diffuseColor;
		//	D3DXVECTOR3 lightDirection;
		//	float specularPower;
		//	D3DXVECTOR4 specularColor;
		//};
		// ------------------------------- //
		bool InitShader(ID3D11Device* dev, Window* wind, wstring vsfilename, wstring psfilename);
		void ReleaseShader();
		void PrintShaderError(ID3D10Blob* datadumb);

		bool SetShaderParams(ID3D11DeviceContext* devcont, D3DXMATRIX worldmatrix, D3DXMATRIX viewmatrix, D3DXMATRIX projectionmatrix, ID3D11ShaderResourceView* texture,
			Float3 lightDirection, Float4 ambientColor, Float4 diffuseColor, Float3 cameraPosition, Float4 specularColor, float specularPower);
		void ShaderRender(ID3D11DeviceContext* devcont, int indexcount);

		ID3D11VertexShader* VertexShader;
		ID3D11PixelShader* PixelShader;
		ID3D11InputLayout* Layout;
		ID3D11Buffer* MatrixBuffer;
		ID3D11SamplerState* SamplerState;
		ID3D11Buffer* CameraBuffer;
		ID3D11Buffer* LightBuffer;

	};
}
#endif