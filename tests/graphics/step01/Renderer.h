#pragma once 

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

	/// 
	void SetEffect( Effect* effect );
	void SetMesh( TriMesh* mesh );

	/// Begin rendering with effect
	void Begin();

	/// Render
	void Render();

	void End();

	/// When lost the device
	void OnLost();

	/// When restored the device
	void OnRestored();

	/// Clean up device
	void Fini();

	int GetError() const;

private:
	Params 		m_params;
	int 		m_error;
	Effect* 	m_effect;
	TriMesh* 	m_mesh;
};

inline 
int Renderer::GetError() const 
{
	return m_error;
}

} // namespace gfx
