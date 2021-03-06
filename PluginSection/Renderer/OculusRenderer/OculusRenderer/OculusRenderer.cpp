/********************************************************************
Vireio Perception: Open-Source Stereoscopic 3D Driver
Copyright (C) 2012 Andres Hernandez

Vireio Oculus Renderer - Oculus Rift Renderer Node Plugin
Copyright (C) 2015 Denis Reischl

File <OculusRenderer.cpp> and
Class <OculusRenderer> :
Copyright (C) 2015 Denis Reischl

The stub class <AQU_Nodus> is the only public class from the Aquilinus 
repository and permitted to be used for open source plugins of any kind. 
Read the Aquilinus documentation for further information.

Vireio Perception Version History:
v1.0.0 2012 by Andres Hernandez
v1.0.X 2013 by John Hicks, Neil Schneider
v1.1.x 2013 by Primary Coding Author: Chris Drain
Team Support: John Hicks, Phil Larkson, Neil Schneider
v2.0.x 2013 by Denis Reischl, Neil Schneider, Joshua Brown
v2.0.4 onwards 2014 by Grant Bagwell, Simon Brown and Neil Schneider

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include"OculusRenderer.h"

#define INTERFACE_IDIRECT3DDEVICE9           8
#define INTERFACE_IDIRECT3DSWAPCHAIN9       15
#define	METHOD_IDIRECT3DDEVICE9_PRESENT     17
#define	METHOD_IDIRECT3DDEVICE9_ENDSCENE    42
#define	METHOD_IDIRECT3DSWAPCHAIN9_PRESENT   3

/**
* Constructor.
***/
OculusRenderer::OculusRenderer():AQU_Nodus(),
	m_hBitmapControl(nullptr),
	m_bControlUpdate(false),
	m_pcTextureLeft(nullptr),
	m_pcTextureRight(nullptr),
	m_pcTextureLeftTest(nullptr),
	m_pcTextureRightTest(nullptr),
	m_pcOculusPixelShader(nullptr),
	m_pcOculusPixelShaderCT(nullptr),
	m_pcOculusVertexShader(nullptr),
	m_pcOculusVertexShaderCT(nullptr),
	m_pcSideBySidePixelShader(nullptr),
	m_pcSideBySidePixelShaderCT(nullptr),
	m_pcDistortionVertexBufferLeft(nullptr),
	m_pcDistortionVertexBufferRight(nullptr),
	m_pcVertexBufferDefault(nullptr),
	m_pcDistortionIndexBufferLeft(nullptr),
	m_pcDistortionIndexBufferRight(nullptr),
	m_pcVertexDecl(nullptr),
	m_psFOVPortLeft(nullptr),
	m_psFOVPortRight(nullptr),
	m_bBuffersConnected(false)
{
	// clear vertex and index buffer descriptions
	ZeroMemory(&m_sVertexBufferDescLeft, sizeof(D3DVERTEXBUFFER_DESC));
	ZeroMemory(&m_sVertexBufferDescRight, sizeof(D3DVERTEXBUFFER_DESC));
	ZeroMemory(&m_sIndexBufferDescLeft, sizeof(D3DINDEXBUFFER_DESC));
	ZeroMemory(&m_sIndexBufferDescRight, sizeof(D3DINDEXBUFFER_DESC));

	// set all fov values to (float)tan(90.0f / 2.0f)
	m_sDefaultFOVPortLeft.LeftTan = (float)tan(90.0f / 2.0f);
	m_sDefaultFOVPortLeft.UpTan = (float)tan(90.0f / 2.0f);
	m_sDefaultFOVPortLeft.RightTan = (float)tan(90.0f / 2.0f);
	m_sDefaultFOVPortLeft.DownTan = (float)tan(90.0f / 2.0f);
	m_sDefaultFOVPortRight.LeftTan = (float)tan(90.0f / 2.0f);
	m_sDefaultFOVPortRight.UpTan = (float)tan(90.0f / 2.0f);
	m_sDefaultFOVPortRight.RightTan = (float)tan(90.0f / 2.0f);
	m_sDefaultFOVPortRight.DownTan = (float)tan(90.0f / 2.0f);
}

/**
* Destructor.
***/
OculusRenderer::~OculusRenderer()
{
	if (m_hBitmapControl) CloseHandle(m_hBitmapControl);

	if (m_pcTextureLeftTest) m_pcTextureLeftTest->Release();
	if (m_pcTextureRightTest) m_pcTextureLeftTest->Release();

	if (m_pcOculusPixelShader) m_pcOculusPixelShader->Release();
	if (m_pcOculusPixelShaderCT) m_pcOculusPixelShaderCT->Release();
	if (m_pcOculusVertexShader) m_pcOculusVertexShader->Release();
	if (m_pcOculusVertexShaderCT) m_pcOculusVertexShaderCT->Release();	
	if (m_pcSideBySidePixelShader) m_pcSideBySidePixelShader->Release();
	if (m_pcSideBySidePixelShaderCT) m_pcSideBySidePixelShaderCT->Release();
	if (m_pcVertexBufferDefault) m_pcVertexBufferDefault->Release();
}

/**
* Return the name of the  Oculus Renderer node.
***/
const char* OculusRenderer::GetNodeType() 
{
	return "Oculus Renderer"; 
}

/**
* Returns a global unique identifier for the Oculus Renderer node.
***/
UINT OculusRenderer::GetNodeTypeId()
{
#define DEVELOPER_IDENTIFIER 2006
#define MY_PLUGIN_IDENTIFIER 258
	return ((DEVELOPER_IDENTIFIER << 16) + MY_PLUGIN_IDENTIFIER);
}

/**
* Returns the name of the category for the Oculus Renderer node.
***/
LPWSTR OculusRenderer::GetCategory()
{
	return L"Renderer";
}

/**
* Returns a logo to be used for the Oculus Renderer node.
***/
HBITMAP OculusRenderer::GetLogo() 
{ 
	HMODULE hModule = GetModuleHandle(L"OculusRenderer.dll");
	HBITMAP hBitmap = LoadBitmap(hModule, MAKEINTRESOURCE(IMG_LOGO01));
	return hBitmap;
}

/** 
* Returns the updated control for the Oculus Renderer node.
* Allways return >nullptr< if there is no update for the control !!
***/
HBITMAP OculusRenderer::GetControl()
{
	if (!m_hBitmapControl)
	{
		// TODO : Design the Oculus Renderer node, both Logo and Control
	}

	return nullptr;
}

/**
* Provides the name of the requested decommander.
***/
LPWSTR OculusRenderer::GetDecommanderName(DWORD dwDecommanderIndex)
{
	switch ((ORN_Decommanders)dwDecommanderIndex)
	{
	case ORN_Decommanders::LeftTexture:
		return L"Left Texture";
	case ORN_Decommanders::RightTexture:
		return L"Right Texture";
	case ORN_Decommanders::DistortionVertexBufferLeft:
		return L"Distortion Vertex Buffer Left";
	case ORN_Decommanders::DistortionVertexBufferRight:
		return L"Distortion Vertex Buffer Right";
	case ORN_Decommanders::DistortionIndexBufferLeft:
		return L"Distortion Index Buffer Left";
	case ORN_Decommanders::DistortionIndexBufferRight:
		return L"Distortion Index Buffer Right";
	case ORN_Decommanders::OculusVertexDeclaration:
		return L"Oculus Vertex Declaration";
	case ORN_Decommanders::DefaultEyeFovLeft:
		return L"Default Eye Fov Left";
	case ORN_Decommanders::DefaultEyeFovRight:
		return L"Default Eye Fov Right";
	}

	return L"";
}

/**
* Returns the plug type for the requested decommander.
***/
DWORD OculusRenderer::GetDecommanderType(DWORD dwDecommanderIndex) 
{
	switch ((ORN_Decommanders)dwDecommanderIndex)
	{
	case ORN_Decommanders::LeftTexture:
		return PNT_IDIRECT3DTEXTURE9_PLUG_TYPE;
	case ORN_Decommanders::RightTexture:
		return PNT_IDIRECT3DTEXTURE9_PLUG_TYPE;
	case ORN_Decommanders::DistortionVertexBufferLeft:
		return PPNT_IDIRECT3DVERTEXBUFFER9_PLUG_TYPE;
	case ORN_Decommanders::DistortionVertexBufferRight:
		return PPNT_IDIRECT3DVERTEXBUFFER9_PLUG_TYPE;
	case ORN_Decommanders::DistortionIndexBufferLeft:
		return PPNT_IDIRECT3DINDEXBUFFER9_PLUG_TYPE;
	case ORN_Decommanders::DistortionIndexBufferRight:
		return PPNT_IDIRECT3DINDEXBUFFER9_PLUG_TYPE;
	case ORN_Decommanders::OculusVertexDeclaration:
		return PPNT_IDIRECT3DVERTEXDECLARATION9_PLUG_TYPE;
	case ORN_Decommanders::DefaultEyeFovLeft:
		return PNT_VECTOR4F_PLUG_TYPE;
	case ORN_Decommanders::DefaultEyeFovRight:
		return PNT_VECTOR4F_PLUG_TYPE;
	}

	return 0;
}

/**
* Sets the input pointer for the requested decommander.
***/
void OculusRenderer::SetInputPointer(DWORD dwDecommanderIndex, void* pData)
{
	if (dwDecommanderIndex < NUMBER_OF_DECOMMANDERS)
		m_paInput[dwDecommanderIndex] = pData;
}

/**
* Oculus Renderer supports D3D 9 Present() and EndScene() calls.
***/
bool OculusRenderer::SupportsD3DMethod(int nD3DVersion, int nD3DInterface, int nD3DMethod)  
{ 
	if ((nD3DVersion >= (int)AQU_DirectXVersion::DirectX_9_0) &&
		(nD3DVersion <= (int)AQU_DirectXVersion::DirectX_9_29))
	{
		if (((nD3DInterface == INTERFACE_IDIRECT3DDEVICE9) &&
			(nD3DMethod == METHOD_IDIRECT3DDEVICE9_PRESENT)) || 
			((nD3DInterface == INTERFACE_IDIRECT3DDEVICE9) &&
			(nD3DMethod == METHOD_IDIRECT3DDEVICE9_ENDSCENE)) ||
			((nD3DInterface == INTERFACE_IDIRECT3DSWAPCHAIN9) &&
			(nD3DMethod == METHOD_IDIRECT3DSWAPCHAIN9_PRESENT)))
			return true;
	}
	return false; 
}

/**
* Render the Oculus Rift View.
***/
void* OculusRenderer::Provoke(void* pThis, int eD3D, int eD3DInterface, int eD3DMethod, DWORD dwNumberConnected, int& nProvokerIndex)	
{
	// return if wrong call
	if ((eD3D >= (int)AQU_DirectXVersion::DirectX_9_0) &&
		(eD3D <= (int)AQU_DirectXVersion::DirectX_9_29))
	{
		if (((eD3DInterface == INTERFACE_IDIRECT3DDEVICE9) &&
			(eD3DMethod == METHOD_IDIRECT3DDEVICE9_PRESENT)) || 
			((eD3DInterface == INTERFACE_IDIRECT3DDEVICE9) &&
			(eD3DMethod == METHOD_IDIRECT3DDEVICE9_ENDSCENE)) ||
			((eD3DInterface == INTERFACE_IDIRECT3DSWAPCHAIN9) &&
			(eD3DMethod == METHOD_IDIRECT3DSWAPCHAIN9_PRESENT)))
		{
			(pThis);
		}
		else return nullptr;
	}
	else
		return nullptr;

	// get input data
	if (m_paInput[(int)ORN_Decommanders::LeftTexture])
		m_pcTextureLeft = *(LPDIRECT3DTEXTURE9*)m_paInput[(int)ORN_Decommanders::LeftTexture];
	else 
		m_pcTextureLeft = nullptr;
	if (m_paInput[(int)ORN_Decommanders::RightTexture])
		m_pcTextureRight = *(LPDIRECT3DTEXTURE9*)m_paInput[(int)ORN_Decommanders::RightTexture];
	else m_pcTextureRight = nullptr;

	if (m_paInput[(int)ORN_Decommanders::DistortionVertexBufferLeft])
		if (*(LPDIRECT3DVERTEXBUFFER9**)m_paInput[(int)ORN_Decommanders::DistortionVertexBufferLeft])
			m_pcDistortionVertexBufferLeft = **(LPDIRECT3DVERTEXBUFFER9**)m_paInput[(int)ORN_Decommanders::DistortionVertexBufferLeft];
		else m_pcDistortionVertexBufferLeft = nullptr;
	else m_pcDistortionVertexBufferLeft = nullptr;
	if (m_paInput[(int)ORN_Decommanders::DistortionVertexBufferRight])
		if (*(LPDIRECT3DVERTEXBUFFER9**)m_paInput[(int)ORN_Decommanders::DistortionVertexBufferRight])
			m_pcDistortionVertexBufferRight = **(LPDIRECT3DVERTEXBUFFER9**)m_paInput[(int)ORN_Decommanders::DistortionVertexBufferRight];
		else m_pcDistortionVertexBufferRight = nullptr;
	else m_pcDistortionVertexBufferRight = nullptr;
	if (m_paInput[(int)ORN_Decommanders::DistortionIndexBufferLeft])
		if (*(LPDIRECT3DINDEXBUFFER9**)m_paInput[(int)ORN_Decommanders::DistortionIndexBufferLeft])
			m_pcDistortionIndexBufferLeft = **(LPDIRECT3DINDEXBUFFER9**)m_paInput[(int)ORN_Decommanders::DistortionIndexBufferLeft];
		else m_pcDistortionIndexBufferLeft = nullptr;
	else m_pcDistortionIndexBufferLeft = nullptr;
	if (m_paInput[(int)ORN_Decommanders::DistortionIndexBufferRight])
		if (*(LPDIRECT3DINDEXBUFFER9**)m_paInput[(int)ORN_Decommanders::DistortionIndexBufferRight])
			m_pcDistortionIndexBufferRight = **(LPDIRECT3DINDEXBUFFER9**)m_paInput[(int)ORN_Decommanders::DistortionIndexBufferRight];
		else m_pcDistortionIndexBufferRight = nullptr;
	else m_pcDistortionIndexBufferRight = nullptr;
	if (m_paInput[(int)ORN_Decommanders::OculusVertexDeclaration])
		if (*(LPDIRECT3DVERTEXDECLARATION9**)m_paInput[(int)ORN_Decommanders::OculusVertexDeclaration])
			m_pcVertexDecl = **(LPDIRECT3DVERTEXDECLARATION9**)m_paInput[(int)ORN_Decommanders::OculusVertexDeclaration];
		else m_pcVertexDecl = nullptr;
	else m_pcVertexDecl = nullptr;
	if (m_paInput[(int)ORN_Decommanders::DefaultEyeFovLeft])
		if (*(ovrFovPort**)m_paInput[(int)ORN_Decommanders::DefaultEyeFovLeft])
			m_psFOVPortLeft = *(ovrFovPort**)m_paInput[(int)ORN_Decommanders::DefaultEyeFovLeft];
		else m_psFOVPortLeft = nullptr;
	else m_psFOVPortLeft = nullptr;
	if (m_paInput[(int)ORN_Decommanders::DefaultEyeFovRight])
		if (*(ovrFovPort**)m_paInput[(int)ORN_Decommanders::DefaultEyeFovRight])
			m_psFOVPortRight = *(ovrFovPort**)m_paInput[(int)ORN_Decommanders::DefaultEyeFovRight];
		else m_psFOVPortRight = nullptr;
	else m_psFOVPortRight = nullptr;

	// get device 
	LPDIRECT3DDEVICE9 pcDevice = nullptr;
	bool bReleaseDevice = false;
	if (eD3DInterface == INTERFACE_IDIRECT3DDEVICE9)
	{
		pcDevice = (LPDIRECT3DDEVICE9)pThis;
	}
	else if (eD3DInterface == INTERFACE_IDIRECT3DSWAPCHAIN9)
	{
		LPDIRECT3DSWAPCHAIN9 pSwapChain = (LPDIRECT3DSWAPCHAIN9)pThis;
		if (!pSwapChain) 
		{
			OutputDebugString(L"Oculus Renderer Node : No swapchain !");
			return nullptr;
		}
		pSwapChain->GetDevice(&pcDevice);
		bReleaseDevice = true;
	}
	if (!pcDevice)
	{
		OutputDebugString(L"Oculus Renderer Node : No device !");
		return nullptr;
	}

	// Original code (LibOVR) :
	// pShaderCode = ShaderCompile("precompiledVertexShaderSrc",VertexShaderSrc,"vs_2_0");
	// pShaderCode = ShaderCompile("precompiledVertexShaderTimewarpSrc",VertexShaderTimewarpSrc,"vs_3_0");
	// pShaderCode = ShaderCompile("precompiledPixelShaderSrc",PixelShaderSrc,"ps_3_0");

	// pixel shader created ?
	if (!m_pcOculusPixelShader)
	{
		LPD3DXBUFFER pShader;

		// compile and create shader
		if (SUCCEEDED(D3DXCompileShader(PixelShaderSrc,strlen(PixelShaderSrc),NULL,NULL,"main","ps_3_0",NULL,&pShader,NULL,&m_pcOculusPixelShaderCT)))
		{
			OutputDebugString(L"Pixel shader compiled!");
			pcDevice->CreatePixelShader((DWORD*)pShader->GetBufferPointer(), &m_pcOculusPixelShader);
		}
	}

	// vertex shader created ?
	if (!m_pcOculusVertexShader)
	{
		LPD3DXBUFFER pShader;

		// compile and create shader
		if (SUCCEEDED(D3DXCompileShader(VertexShaderSrc,strlen(VertexShaderSrc),NULL,NULL,"main","vs_2_0",NULL,&pShader,NULL,&m_pcOculusVertexShaderCT)))
		{
			OutputDebugString(L"Vertex shader compiled!");
			pcDevice->CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &m_pcOculusVertexShader);
		}
	}

	// side by side pixel shader ?
	if (!m_pcSideBySidePixelShader)
	{
		LPD3DXBUFFER pShader;

		// compile and create shader
		if (SUCCEEDED(D3DXCompileShader(PixelShaderSrcSideBySide,strlen(PixelShaderSrcSideBySide),NULL,NULL,"SBS","ps_2_0",NULL,&pShader,NULL,&m_pcSideBySidePixelShaderCT)))
		{
			OutputDebugString(L"Pixel shader compiled!");
			pcDevice->CreatePixelShader((DWORD*)pShader->GetBufferPointer(), &m_pcSideBySidePixelShader);
		}

	}

	// test textures created ?
	if ((!m_pcTextureRightTest) || (!m_pcTextureLeftTest))
	{
		HMODULE hModule = GetModuleHandle(L"OculusRenderer.dll");

		if (!m_pcTextureLeftTest)
		{
			// create a test texture
			if (SUCCEEDED(D3DXCreateTextureFromResource(pcDevice, hModule, MAKEINTRESOURCE(IMG_BACKGROUND01), &m_pcTextureLeftTest)))
				OutputDebugString(L"Texture created !");
			else m_pcTextureLeftTest = nullptr;
		}
		if (!m_pcTextureRightTest)
		{
			// create a test texture
			if (SUCCEEDED(D3DXCreateTextureFromResource(pcDevice, hModule, MAKEINTRESOURCE(IMG_BACKGROUND02), &m_pcTextureRightTest)))
				OutputDebugString(L"Texture created !");
			else m_pcTextureRightTest = nullptr;
		}
	}

	// default vertex buffer ?
	if (!m_pcVertexBufferDefault)
	{
		InitDefaultVertexBuffer(pcDevice);
		if (!m_pcVertexBufferDefault) return nullptr;
	}

	// vertex buffers ? index buffers ? not all present ?
	if ((!m_pcDistortionVertexBufferLeft) || (!m_pcDistortionVertexBufferRight) ||
		(!m_pcDistortionIndexBufferLeft) || (!m_pcDistortionIndexBufferRight) ||
		(!m_pcVertexDecl))
	{
		if (m_bBuffersConnected)
		{
			// clear all descriptions for safety
			ZeroMemory(&m_sVertexBufferDescLeft, sizeof(D3DVERTEXBUFFER_DESC));
			ZeroMemory(&m_sVertexBufferDescRight, sizeof(D3DVERTEXBUFFER_DESC));
			ZeroMemory(&m_sIndexBufferDescLeft, sizeof(D3DINDEXBUFFER_DESC));
			ZeroMemory(&m_sIndexBufferDescRight, sizeof(D3DINDEXBUFFER_DESC));

			m_bBuffersConnected = false;
		}
	}
	else
	{
		m_bBuffersConnected = true;

		// vertex buffer description ? left
		if (!m_sVertexBufferDescLeft.Size)
			m_pcDistortionVertexBufferLeft->GetDesc(&m_sVertexBufferDescLeft);

		// vertex buffer length matches oculus vertex type ?
		if ((m_sVertexBufferDescLeft.Size % sizeof(ovrDistortionVertex)) != 0)
		{
			OutputDebugString(L"OculusRenderer Node : Connected vertex buffer size mismatch !");
			return nullptr;
		}

		// and right
		if (!m_sVertexBufferDescRight.Size)
			m_pcDistortionVertexBufferRight->GetDesc(&m_sVertexBufferDescRight);

		// vertex buffer length matches oculus vertex type ?
		if ((m_sVertexBufferDescRight.Size % sizeof(ovrDistortionVertex)) != 0)
		{
			OutputDebugString(L"OculusRenderer Node : Connected vertex buffer size mismatch !");
			return nullptr;
		}

		// index buffer ?
		if ((!m_pcDistortionIndexBufferLeft) || (!m_pcDistortionIndexBufferRight))
			return nullptr;

		// index buffer description ?
		if (!m_sIndexBufferDescLeft.Size)
		{
			m_pcDistortionIndexBufferLeft->GetDesc(&m_sIndexBufferDescLeft);

			// index buffer length matches vertex buffer size ? TODO !!
			/*if ()
			{
			OutputDebugString(L"OculusRenderer Node : Connected index buffer size mismatch !");
			return nullptr;
			}*/
		}
		if (!m_sIndexBufferDescRight.Size)
		{
			m_pcDistortionIndexBufferRight->GetDesc(&m_sIndexBufferDescRight);

			// index buffer length matches vertex buffer size ? TODO !!
			/*if ()
			{
			OutputDebugString(L"OculusRenderer Node : Connected index buffer size mismatch !");
			return nullptr;
			}*/
		}
	}

	// start to render
	pcDevice->BeginScene();

	// save states
	IDirect3DStateBlock9* pStateBlock = nullptr;
	pcDevice->CreateStateBlock(D3DSBT_ALL, &pStateBlock);

	// set ALL render states to default
	SetAllRenderStatesDefault(pcDevice);

	// set states
	pcDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pcDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pcDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CONSTANT);
	pcDevice->SetTextureStageState(0, D3DTSS_CONSTANT, 0xffffffff);
	pcDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pcDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pcDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pcDevice->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 0);
	pcDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pcDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	pcDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	pcDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	pcDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pcDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	D3DCOLOR clearColor = D3DCOLOR_RGBA(0, 0, 0, 0);

	pcDevice->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 0, 0);

	// required fields
	D3DSURFACE_DESC sSurfaceDesc;
	ovrSizei sTextureSize;
	ovrRecti sRenderViewport;
	ovrVector2f UVScaleOffset[2];

	// LEFT EYE :

	// left eye, first set stream source, indices
	if (m_bBuffersConnected)
	{
		// no timewarp here, use standard vertex shader, set pixel shader, set vertex declaration
		pcDevice->SetVertexShader( m_pcOculusVertexShader);
		pcDevice->SetPixelShader( m_pcOculusPixelShader );
		pcDevice->SetVertexDeclaration( m_pcVertexDecl );

		// set texture
		if (m_pcTextureLeft)
			pcDevice->SetTexture( 0, m_pcTextureLeft );
		else if (m_pcTextureLeftTest)
			pcDevice->SetTexture( 0, m_pcTextureLeftTest );
		else pcDevice->SetTexture( 0, 0);

		// get texture size
		if (m_pcTextureLeft)
			m_pcTextureLeft->GetLevelDesc(0, &sSurfaceDesc);
		else if (m_pcTextureLeftTest)
			m_pcTextureLeftTest->GetLevelDesc(0, &sSurfaceDesc);
		else ZeroMemory(&sSurfaceDesc, sizeof(D3DSURFACE_DESC));
		sTextureSize.w = (int)sSurfaceDesc.Width;
		sTextureSize.h = (int)sSurfaceDesc.Height;

		// set render viewport size the same size as the texture size (!)
		sRenderViewport.Pos.x = 0;
		sRenderViewport.Pos.y = 0;
		sRenderViewport.Size.w = sTextureSize.w;
		sRenderViewport.Size.h = sTextureSize.h;

		// get and set scale and offset
		if (m_psFOVPortLeft)
			ovrHmd_GetRenderScaleAndOffset(*m_psFOVPortLeft, sTextureSize, sRenderViewport, UVScaleOffset);
		else
			ovrHmd_GetRenderScaleAndOffset(m_sDefaultFOVPortLeft, sTextureSize, sRenderViewport, UVScaleOffset);
		pcDevice->SetVertexShaderConstantF( 0, ( FLOAT* )&UVScaleOffset[0], 1 );
		pcDevice->SetVertexShaderConstantF( 2, ( FLOAT* )&UVScaleOffset[1], 1 );

		pcDevice->SetStreamSource( 0, m_pcDistortionVertexBufferLeft,0, sizeof(ovrDistortionVertex) );
		pcDevice->SetIndices( m_pcDistortionIndexBufferLeft);

		// draw
		pcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0, m_sVertexBufferDescLeft.Size / sizeof(ovrDistortionVertex) , 0, m_sIndexBufferDescLeft.Size / 6 );
	}
	else
	{
		pcDevice->SetSamplerState(1, D3DSAMP_SRGBTEXTURE, 0);
		pcDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		pcDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		pcDevice->SetSamplerState(1, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
		pcDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
		pcDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		pcDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		// side by side render
		pcDevice->SetVertexShader(NULL);
		pcDevice->SetPixelShader(m_pcSideBySidePixelShader);
		pcDevice->SetVertexDeclaration( NULL );
		pcDevice->SetFVF(D3DFVF_TEXVERTEX);

		// set textures
		if (m_pcTextureLeft)
			pcDevice->SetTexture( 0, m_pcTextureLeft );
		else if (m_pcTextureLeftTest)
			pcDevice->SetTexture( 0, m_pcTextureLeftTest );
		else pcDevice->SetTexture( 0, 0);
		if (m_pcTextureRight)
			pcDevice->SetTexture( 1, m_pcTextureRight );
		else if (m_pcTextureRightTest)
			pcDevice->SetTexture( 1, m_pcTextureRightTest );
		else pcDevice->SetTexture( 1, 0);

		// set stream and draw
		pcDevice->SetStreamSource( 0, m_pcVertexBufferDefault,0, sizeof(TEXVERTEX) );
		pcDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	}

	// RIGHT EYE:
	if (m_bBuffersConnected)
	{
		// set texture
		if (m_pcTextureRight)
			pcDevice->SetTexture( 0, m_pcTextureRight );
		else if (m_pcTextureRightTest)
			pcDevice->SetTexture( 0, m_pcTextureRightTest );
		else pcDevice->SetTexture( 0, 0);

		// get texture size
		if (m_pcTextureRight)
			m_pcTextureRight->GetLevelDesc(0, &sSurfaceDesc);
		else if (m_pcTextureRightTest)
			m_pcTextureRightTest->GetLevelDesc(0, &sSurfaceDesc);
		else ZeroMemory(&sSurfaceDesc, sizeof(D3DSURFACE_DESC));
		sTextureSize.w = (int)sSurfaceDesc.Width;
		sTextureSize.h = (int)sSurfaceDesc.Height;

		// get and set scale and offset
		if (m_psFOVPortRight)
			ovrHmd_GetRenderScaleAndOffset(*m_psFOVPortRight, sTextureSize, sRenderViewport, UVScaleOffset);
		else
			ovrHmd_GetRenderScaleAndOffset(m_sDefaultFOVPortRight, sTextureSize, sRenderViewport, UVScaleOffset);
		pcDevice->SetVertexShaderConstantF( 0, ( FLOAT* )&UVScaleOffset[0], 1 );
		pcDevice->SetVertexShaderConstantF( 2, ( FLOAT* )&UVScaleOffset[1], 1 );

		// set stream source, indices, draw
		pcDevice->SetStreamSource( 0, m_pcDistortionVertexBufferRight,0, sizeof(ovrDistortionVertex) );
		pcDevice->SetIndices( m_pcDistortionIndexBufferRight);

		// draw
		pcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0, m_sVertexBufferDescRight.Size / sizeof(ovrDistortionVertex) , 0, m_sIndexBufferDescRight.Size / 6 );
	}

	pcDevice->EndScene();

	// apply and release state block
	if (pStateBlock)
	{
		pStateBlock->Apply();
		pStateBlock->Release();
	}

	// release device if provided by swapchain
	if (bReleaseDevice) pcDevice->Release();

	return nullptr;
}

/**
* Sets all Direct3D 9 render states to their default values.
* Use this function only if a game does not want to render.
***/
void OculusRenderer::SetAllRenderStatesDefault(LPDIRECT3DDEVICE9 pcDevice)
{
	// set all Direct3D 9 RenderStates to default values
	float fData = 0.0f;
	double dData = 0.0f;

	pcDevice->SetRenderState(D3DRS_ZENABLE                     , D3DZB_TRUE);
	pcDevice->SetRenderState(D3DRS_FILLMODE                    , D3DFILL_SOLID);
	pcDevice->SetRenderState(D3DRS_SHADEMODE                   , D3DSHADE_GOURAUD);
	pcDevice->SetRenderState(D3DRS_ZWRITEENABLE                , TRUE);
	pcDevice->SetRenderState(D3DRS_ALPHATESTENABLE             , FALSE);
	pcDevice->SetRenderState(D3DRS_LASTPIXEL                   , TRUE);
	pcDevice->SetRenderState(D3DRS_SRCBLEND                    , D3DBLEND_ONE);
	pcDevice->SetRenderState(D3DRS_DESTBLEND                   , D3DBLEND_ZERO);
	pcDevice->SetRenderState(D3DRS_CULLMODE                    , D3DCULL_CCW);
	pcDevice->SetRenderState(D3DRS_ZFUNC                       , D3DCMP_LESSEQUAL);
	pcDevice->SetRenderState(D3DRS_ALPHAREF                    , 0);
	pcDevice->SetRenderState(D3DRS_ALPHAFUNC                   , D3DCMP_ALWAYS);
	pcDevice->SetRenderState(D3DRS_DITHERENABLE                , FALSE);
	pcDevice->SetRenderState(D3DRS_ALPHABLENDENABLE            , FALSE);
	pcDevice->SetRenderState(D3DRS_FOGENABLE                   , FALSE);
	pcDevice->SetRenderState(D3DRS_SPECULARENABLE              , FALSE);
	pcDevice->SetRenderState(D3DRS_FOGCOLOR                    , 0);
	pcDevice->SetRenderState(D3DRS_FOGTABLEMODE                , D3DFOG_NONE);
	fData = 0.0f;
	pcDevice->SetRenderState(D3DRS_FOGSTART                    , *((DWORD*)&fData));
	fData = 1.0f;
	pcDevice->SetRenderState(D3DRS_FOGEND                      , *((DWORD*)&fData));
	fData = 1.0f;
	pcDevice->SetRenderState(D3DRS_FOGDENSITY                  , *((DWORD*)&fData));
	pcDevice->SetRenderState(D3DRS_RANGEFOGENABLE              , FALSE);
	pcDevice->SetRenderState(D3DRS_STENCILENABLE               , FALSE);
	pcDevice->SetRenderState(D3DRS_STENCILFAIL                 , D3DSTENCILOP_KEEP);
	pcDevice->SetRenderState(D3DRS_STENCILZFAIL                , D3DSTENCILOP_KEEP);
	pcDevice->SetRenderState(D3DRS_STENCILPASS                 , D3DSTENCILOP_KEEP);
	pcDevice->SetRenderState(D3DRS_STENCILFUNC                 , D3DCMP_ALWAYS);
	pcDevice->SetRenderState(D3DRS_STENCILREF                  , 0);
	pcDevice->SetRenderState(D3DRS_STENCILMASK                 , 0xFFFFFFFF);
	pcDevice->SetRenderState(D3DRS_STENCILWRITEMASK            , 0xFFFFFFFF);
	pcDevice->SetRenderState(D3DRS_TEXTUREFACTOR               , 0xFFFFFFFF);
	pcDevice->SetRenderState(D3DRS_WRAP0                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP1                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP2                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP3                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP4                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP5                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP6                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP7                       , 0);
	pcDevice->SetRenderState(D3DRS_CLIPPING                    , TRUE);
	pcDevice->SetRenderState(D3DRS_LIGHTING                    , TRUE);
	pcDevice->SetRenderState(D3DRS_AMBIENT                     , 0);
	pcDevice->SetRenderState(D3DRS_FOGVERTEXMODE               , D3DFOG_NONE);
	pcDevice->SetRenderState(D3DRS_COLORVERTEX                 , TRUE);
	pcDevice->SetRenderState(D3DRS_LOCALVIEWER                 , TRUE);
	pcDevice->SetRenderState(D3DRS_NORMALIZENORMALS            , FALSE);
	pcDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE       , D3DMCS_COLOR1);
	pcDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE      , D3DMCS_COLOR2);
	pcDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE       , D3DMCS_MATERIAL);
	pcDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE      , D3DMCS_MATERIAL);
	pcDevice->SetRenderState(D3DRS_VERTEXBLEND                 , D3DVBF_DISABLE);
	pcDevice->SetRenderState(D3DRS_CLIPPLANEENABLE             , 0);
	pcDevice->SetRenderState(D3DRS_POINTSIZE                   , 64);
	fData = 1.0f;
	pcDevice->SetRenderState(D3DRS_POINTSIZE_MIN               , *((DWORD*)&fData));
	pcDevice->SetRenderState(D3DRS_POINTSPRITEENABLE           , FALSE);
	pcDevice->SetRenderState(D3DRS_POINTSCALEENABLE            , FALSE);
	fData = 1.0f;
	pcDevice->SetRenderState(D3DRS_POINTSCALE_A                , *((DWORD*)&fData));
	fData = 0.0f;
	pcDevice->SetRenderState(D3DRS_POINTSCALE_B                , *((DWORD*)&fData));
	fData = 0.0f;
	pcDevice->SetRenderState(D3DRS_POINTSCALE_C                , *((DWORD*)&fData));
	pcDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS        , TRUE);
	pcDevice->SetRenderState(D3DRS_MULTISAMPLEMASK             , 0xFFFFFFFF);
	pcDevice->SetRenderState(D3DRS_PATCHEDGESTYLE              , D3DPATCHEDGE_DISCRETE);
	pcDevice->SetRenderState(D3DRS_DEBUGMONITORTOKEN           , D3DDMT_ENABLE);
	dData = 64.0;
	pcDevice->SetRenderState(D3DRS_POINTSIZE_MAX               , *((DWORD*)&dData));
	pcDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE    , FALSE);
	pcDevice->SetRenderState(D3DRS_COLORWRITEENABLE            , 0x0000000F);
	fData = 0.0f;
	pcDevice->SetRenderState(D3DRS_TWEENFACTOR                 , *((DWORD*)&fData));
	pcDevice->SetRenderState(D3DRS_BLENDOP                     , D3DBLENDOP_ADD);
	pcDevice->SetRenderState(D3DRS_POSITIONDEGREE              , D3DDEGREE_CUBIC);
	pcDevice->SetRenderState(D3DRS_NORMALDEGREE                , D3DDEGREE_LINEAR );
	pcDevice->SetRenderState(D3DRS_SCISSORTESTENABLE           , FALSE);
	pcDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS         , 0);
	pcDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE       , FALSE);
	fData = 1.0f;
	pcDevice->SetRenderState(D3DRS_MINTESSELLATIONLEVEL        , *((DWORD*)&fData));
	fData = 1.0f;
	pcDevice->SetRenderState(D3DRS_MAXTESSELLATIONLEVEL        , *((DWORD*)&fData));
	fData = 0.0f;
	pcDevice->SetRenderState(D3DRS_ADAPTIVETESS_X              , *((DWORD*)&fData));
	fData = 0.0f;
	pcDevice->SetRenderState(D3DRS_ADAPTIVETESS_Y              , *((DWORD*)&fData));
	fData = 1.0f;
	pcDevice->SetRenderState(D3DRS_ADAPTIVETESS_Z              , *((DWORD*)&fData));
	fData = 0.0f;
	pcDevice->SetRenderState(D3DRS_ADAPTIVETESS_W              , *((DWORD*)&fData));
	pcDevice->SetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION  , FALSE);
	pcDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE         , FALSE);
	pcDevice->SetRenderState(D3DRS_CCW_STENCILFAIL             , D3DSTENCILOP_KEEP);
	pcDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL            , D3DSTENCILOP_KEEP);
	pcDevice->SetRenderState(D3DRS_CCW_STENCILPASS             , D3DSTENCILOP_KEEP);
	pcDevice->SetRenderState(D3DRS_CCW_STENCILFUNC             , D3DCMP_ALWAYS);
	pcDevice->SetRenderState(D3DRS_COLORWRITEENABLE1           , 0x0000000f);
	pcDevice->SetRenderState(D3DRS_COLORWRITEENABLE2           , 0x0000000f);
	pcDevice->SetRenderState(D3DRS_COLORWRITEENABLE3           , 0x0000000f);
	pcDevice->SetRenderState(D3DRS_BLENDFACTOR                 , 0xffffffff);
	pcDevice->SetRenderState(D3DRS_SRGBWRITEENABLE             , 0);
	pcDevice->SetRenderState(D3DRS_DEPTHBIAS                   , 0);
	pcDevice->SetRenderState(D3DRS_WRAP8                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP9                       , 0);
	pcDevice->SetRenderState(D3DRS_WRAP10                      , 0);
	pcDevice->SetRenderState(D3DRS_WRAP11                      , 0);
	pcDevice->SetRenderState(D3DRS_WRAP12                      , 0);
	pcDevice->SetRenderState(D3DRS_WRAP13                      , 0);
	pcDevice->SetRenderState(D3DRS_WRAP14                      , 0);
	pcDevice->SetRenderState(D3DRS_WRAP15                      , 0);
	pcDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE    , FALSE);
	pcDevice->SetRenderState(D3DRS_SRCBLENDALPHA               , D3DBLEND_ONE);
	pcDevice->SetRenderState(D3DRS_DESTBLENDALPHA              , D3DBLEND_ZERO);
	pcDevice->SetRenderState(D3DRS_BLENDOPALPHA                , D3DBLENDOP_ADD);
}

/**
* Inits a full screen vertex buffer containing 4 vertices.
***/
void OculusRenderer::InitDefaultVertexBuffer(LPDIRECT3DDEVICE9 pcDevice)
{
	// get the viewport
	D3DVIEWPORT9 sViewport;
	pcDevice->GetViewport(&sViewport);

	// create vertex buffer
	pcDevice->CreateVertexBuffer(sizeof(TEXVERTEX) * 4, NULL,
		D3DFVF_TEXVERTEX, D3DPOOL_MANAGED, &m_pcVertexBufferDefault, NULL);

	// ..and lock
	TEXVERTEX* vertices;
	m_pcVertexBufferDefault->Lock(0, 0, (void**)&vertices, NULL);

	RECT* rDest = new RECT();
	rDest->left = 0;
	rDest->right = int(sViewport.Width);
	rDest->top = 0;
	rDest->bottom = int(sViewport.Height);

	//Setup vertices
	vertices[0].x = (float) rDest->left - 0.5f;
	vertices[0].y = (float) rDest->top - 0.5f;
	vertices[0].z = 0.0f;
	vertices[0].rhw = 1.0f;
	vertices[0].u = 0.0f;
	vertices[0].v = 0.0f;

	vertices[1].x = (float) rDest->right - 0.5f;
	vertices[1].y = (float) rDest->top - 0.5f;
	vertices[1].z = 0.0f;
	vertices[1].rhw = 1.0f;
	vertices[1].u = 1.0f;
	vertices[1].v = 0.0f;

	vertices[2].x = (float) rDest->right - 0.5f;
	vertices[2].y = (float) rDest->bottom - 0.5f;
	vertices[2].z = 0.0f;
	vertices[2].rhw = 1.0f;
	vertices[2].u = 1.0f;	
	vertices[2].v = 1.0f;

	vertices[3].x = (float) rDest->left - 0.5f;
	vertices[3].y = (float) rDest->bottom - 0.5f;
	vertices[3].z = 0.0f;
	vertices[3].rhw = 1.0f;
	vertices[3].u = 0.0f;
	vertices[3].v = 1.0f;

	m_pcVertexBufferDefault->Unlock();
}