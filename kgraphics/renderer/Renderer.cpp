#include "stdafx.h"

#include "Renderer.h"

#include <dxerr.h>

using namespace gfx;

Renderer::Renderer()
: m_params()
, m_direct3d( 0 )
, m_device( 0 )
, m_error( 0 )
{
}

Renderer::~Renderer()
{
	Fini();
}

bool Renderer::Init( const Params& params )
{
	m_params = params;

	bool rc = createDirect3d();
	if ( !rc ) return false; 

	rc = createDevice();
	if ( !rc ) return false; 

	return true;
}

bool Renderer::BeginScene()
{
	m_device->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

	return SUCCEEDED( m_device->BeginScene() );	
}

void Renderer::SetStreamSource( VertexBuffer* vbuf )
{
	K_ASSERT( vbuf != 0 );

	HRESULT rc = m_device->SetStreamSource( 0, vbuf->GetBuffer(), 0, vbuf->GetStride() );

	if ( FAILED( rc ) )
	{
		LogError( rc );
	}
}

void Renderer::SetFvf( int fvf )
{
	m_device->SetFVF( fvf );
}

void Renderer::SetIndices( IndexBuffer* ibuf )
{
	K_ASSERT( ibuf != 0 );

	HRESULT rc = m_device->SetIndices( ibuf->GetBuffer() );

	if ( FAILED( rc ) )
	{
		LogError( rc );
	}
}

void Renderer::DrawIndexed( int bi, uint mi, uint nv, uint si, uint pc )
{
	m_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, bi, mi, nv, si, pc );
}

void Renderer::EndScene()
{
	m_device->EndScene();
	m_device->Present( 0, 0, 0, 0 );
}

void Renderer::OnLost()
{
	// TODO: resource handling
}

void Renderer::OnRestored()
{
	// TODO: resource handling
}

void Renderer::Fini()
{
	releaseDevice();
	releaseDirect3d();
}

void Renderer::LogError( HRESULT hr )
{
	char buf[2048];
	
#pragma warning( disable : 4996 )
	sprintf(buf, 
		    "Error: %s error description: %s\n",
		    DXGetErrorString(hr),
			DXGetErrorDescription(hr));

	OutputDebugString(buf);
}

VertexBuffer* 
Renderer::CreateVertexBuffer( uint triangleCount, uint stride, uint fvf )
{
	IDirect3DVertexBuffer9* vb; 

	m_device->CreateVertexBuffer( triangleCount*stride, 
                                  D3DUSAGE_WRITEONLY, 
								  fvf,
								  D3DPOOL_MANAGED, 
								  &vb, 
								  NULL );

	if ( vb != 0 )
	{
		return new VertexBuffer( vb, triangleCount, stride, fvf );
	}

	return (VertexBuffer*)0;
}

IndexBuffer* 
Renderer::CreateIndexBuffer( uint faceCount )
{
	IDirect3DIndexBuffer9* ib; 

	m_device->CreateIndexBuffer( faceCount*3*2, 
		                         D3DUSAGE_WRITEONLY, 
								 D3DFMT_INDEX16, 
								 D3DPOOL_MANAGED, 
								 &ib, 
								 NULL );

	if ( ib != 0 )
	{
		return new IndexBuffer( ib, faceCount );
	}

	return (IndexBuffer*)0;
}

Effect*	
Renderer::CreateEffect( const std::string& file )
{
	ID3DXEffect* effect = 0;
	LPD3DXBUFFER error = 0;

	if ( SUCCEEDED( D3DXCreateEffectFromFile( 
						m_device, 
						file.c_str(), 
						0, 
						0, 
						0, 
						0, 
						&effect, 
						&error
					) ) )
	{
		return new Effect( effect );
	}	

	if ( error )
	{
		MessageBoxA( NULL, (LPCSTR)error->GetBufferPointer(), "Error", MB_OK );
	}

	return (Effect*)0;
}

Texture* 		
Renderer::CreateTexture( const std::string& file )
{
	LPDIRECT3DTEXTURE9 tex; 

	if ( SUCCEEDED( D3DXCreateTextureFromFile(
						m_device, 
						file.c_str(), 
						&tex ) ) )
	{
		return new Texture( tex );
	}

	return (Texture*)0;
}

bool Renderer::createDirect3d()
{
	m_direct3d = Direct3DCreate9( D3D_SDK_VERSION );

	return m_direct3d != 0;
}

bool Renderer::createDevice()
{
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );

    d3dpp.Windowed			= TRUE;
    d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat	= D3DFMT_X8R8G8B8;		// set the back buffer format to 32-bit. TODO: Get desktop bpp 
    d3dpp.BackBufferWidth	= m_params.w;			// set the width of the buffer
    d3dpp.BackBufferHeight	= m_params.h;			// set the height of the buffer
	d3dpp.BackBufferCount	= 1;

    if( FAILED( m_direct3d->CreateDevice( D3DADAPTER_DEFAULT, 
										  D3DDEVTYPE_HAL, 
										  m_params.hwnd,
                                      	  D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      	  &d3dpp, 
										  &m_device) ) )
    {
		return false; 
    }

	return true;
}

void Renderer::releaseDirect3d()
{
	if ( m_direct3d != 0 )
	{
		m_direct3d->Release();
		m_direct3d = 0;
	}
}

void Renderer::releaseDevice()
{
	if ( m_device != 0 )
	{
		m_device->Release();
		m_device = 0;
	}
}


/* 
 Shader를 일반화 시키려면 Constant의 지정을 Semantic, Annotation으로 얻어와서 지정하기와 
 버텍스 형식을 일반화 시켜서 지정하는 게 필요하다. 

 shader가 동작하면 이 작업을 해서 좀 더 읿반화된 shader 기능을 만든다. 


 D3DVERTEXELEMENT9 g_aVertDecl[] =
{
{ 0, sizeof( float ) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
{ 0, sizeof( float ) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
{ 0, sizeof( float ) * 6, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
{ 0, sizeof( float ) * 8, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
{ 0, sizeof( float ) * 11, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
D3DDECL_END()
};

*/ 
