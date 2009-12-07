#pragma once 

#include <d3d9.h>

namespace gfx {

/**
 * @class IndexBuffer
 */
class IndexBuffer
{
public:
	IndexBuffer( IDirect3DIndexBuffer9* ib, uint faceCount );
	~IndexBuffer();

	/**
	 * Update index buffer memory 
	 *
	 * @param v 		Memory holding indices
	 * @param faceCount Face count 
	 */
	bool Update( byte* v, uint faceCount );

	IDirect3DIndexBuffer9* GetBuffer();
	uint GetFaceCount() const;

private:
	IDirect3DIndexBuffer9* 	m_ib; 
	uint m_faceCont; 
}; 

inline
IDirect3DIndexBuffer9* 
IndexBuffer::GetBuffer()
{
	return m_ib;
}

inline
uint
IndexBuffer::GetFaceCount() const 
{
	return m_faceCont;
}

} // namespace gfx
