#pragma once 

namespace gfx
{

/**
 * @class Effect
 *
 * 
 */
class Effect 
{
public:
	Effect();
	~Effect();

	/// create effect from file 
	bool Init( const std::string& file );	

	// set matrix, texture, and other variables  
	void SetMatrix();
	void SetTexture();

	bool Begin();
	void End();

	void OnLost();
	void OnRestored();

	void Fini();
};

} // namespace gfx
