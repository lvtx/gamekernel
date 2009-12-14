#include "stdafx.h"

#include "Renderer.h"

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

	m_device->SetStreamSource( 0, vbuf->GetBuffer(), 0, vbuf->GetStride() );
}

void Renderer::SetIndices( IndexBuffer* ibuf )
{
	K_ASSERT( ibuf != 0 );

	m_device->SetIndices( ibuf->GetBuffer() );
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

	MessageBoxA( NULL, (LPCSTR)error->GetBufferPointer(), "Error", MB_OK );

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
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

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
