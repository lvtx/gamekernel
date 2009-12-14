#pragma once 

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#include "Texture.h"

namespace gfx
{
class Texture;

/**
 * @class Effect
 *
 * 
 */
class Effect 
{
public:
	Effect( ID3DXEffect* effect );
	~Effect();
	
	bool SetMatrix( const std::string& name, D3DXMATRIX* matrix );
	bool SetTexture( const std::string& name, Texture* tex );
	bool SetValue( const std::string& name, void* v, uint size );

	bool SetTechnique( const std::string& tech );
	bool Begin( uint& passes ); 
	bool BeginPass( uint pass );
	bool EndPass();
	bool End();
	
	void OnLost();
	void OnRestored();

private:
	ID3DXEffect* 	m_effect;
};

} // namespace gfx
