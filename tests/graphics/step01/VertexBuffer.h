#pragma once 

namespace gfx {

class Renderer;

/**
 * @class VertexBuffer
 * 
 */
class VertexBuffer
{
public:
	VertexBuffer( IDirect3DVertexBuffer9* vb, uint triangleCount, uint stride, uint fvf );
	~VertexBuffer();

	/**
	 * Update vertex buffer memory 
	 * @param v 			memory holding vertices
	 * @param triangleCount The number of triangles
	 * @param stride  		stride of a vertex
	 */
	bool Update( byte* v, uint triangleCount, uint stride ); 

	IDirect3DVertexBuffer9* GetBuffer();
	uint GetTriangleCount() const;
	uint GetStride() const;
	uint GetFvf() const;

private:
	IDirect3DVertexBuffer9* m_vb; 
	uint m_triangleCount; 
	uint m_stride; 
	uint m_fvf;
};

inline
IDirect3DVertexBuffer9* 
VertexBuffer::GetBuffer()
{
	return m_vb;
}

inline
uint 
VertexBuffer::GetTriangleCount() const
{
	return m_triangleCount;
}

inline
uint 
VertexBuffer::GetStride() const
{
	return m_stride;
}

inline
uint 
VertexBuffer::GetFvf() const
{
	return m_fvf;
}

} // namespace gfx
