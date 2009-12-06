#pragma once 

/**
 * @class Model 
 * 
 * Has mesh and submeshes
 */
class Model 
{
public:
	Model();
	virtual ~Model();

	virtual bool Load( const std::string& file );
	virtual void Draw( Renderer* renderer );
	virtual void Unload();
};