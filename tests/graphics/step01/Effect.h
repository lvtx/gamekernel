#pragma once 

#include <d3d9.h>
#include <d3dx9effect.h>
#include <string>

namespace gfx
{
class Renderer;
class Model; 

/**
 * @class Effect
 *
 * 
 */
class Effect 
{
public:
	Effect( ID3DXEFFECT* effect );
	~Effect();

	bool SetMatrix( const std::string& name, D3DXMATRIXA16* matrix );
	bool SetTexture( const std::string& name, Texture* tex );
	bool SetValue( const std::string& name, void* v, uint size );

	bool Begin( uint& passes ); 
	bool BeginPass( uint pass );
	bool EndPass();
	bool End();
	
	void OnLost();
	void OnRestored();

private:
	ID3DXEFFECT* 	m_effect;
};

} // namespace gfx
