#pragma once 

#include "Effect.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace gfx
{
/**
 * @class Renderer 
 *
 * A simple wrapper for D3D device
 */
class Renderer 
{
public:
	struct Params 
	{
		int		w; 
		int		h; 
		bool	windowed;
		HWND	hwnd;

		Params() : w(1024), h(768), windowed( true ), hwnd( NULL ) {}
	};

public:	
	Renderer();
	~Renderer();

	/// Initialize renderer
	bool Init( const Params& params );

	/// Clear and BeginScene
	bool BeginScene();

	/// Set stream source to draw
	void SetStreamSource( VertexBuffer* vbuf );

	void SetFvf( int fvf );

	/// Set indices to draw
	void SetIndices( IndexBuffer* ibuf );

	/// DIP call for triangles
	void DrawIndexed( int baseIndex,
					  uint minIndex, 
					  uint numVertices, 
					  uint startIndex, 
					  uint primitiveCount );

	/// EndScene and Present
	void EndScene();

	/// EndScene and Present
	void OnLost();

	/// When restored the device
	void OnRestored();

	/// Clean up device
	void Fini();

	int GetError() const;

	VertexBuffer* 	CreateVertexBuffer( uint triangleCount, uint stride, uint fvf );
	IndexBuffer* 	CreateIndexBuffer( uint faceCount );
	Effect* 		CreateEffect( const std::string& file );
	Texture* 		CreateTexture( const std::string& file );

private:
	bool createDirect3d();
	bool createDevice();
	void releaseDirect3d();
	void releaseDevice();

private:
	Params 				m_params;
	IDirect3D9* 		m_direct3d;
	IDirect3DDevice9* 	m_device;
	int 				m_error;
};

inline 
int Renderer::GetError() const 
{
	return m_error;
}

} // namespace gfx
