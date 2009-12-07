#pragma once 

namespace gfx {

class Renderer;

/**
 * @class Model 
 * 
 * Has mesh and submeshes
 */
class Model : public Drawable
{
public:
	enum 
	{
		MODEL_INVALID, 
		MODEL_BASIC, 
		MODEL_SKIN, 
		MODEL_SKIN_BUMP
	};

public:
	Model();
	virtual ~Model();

	virtual bool Load( Renderer* renderer, const std::string& file );
	virtual void Unload();

	uint GetType() const;

protected:
	uint 	m_type;
};

inline
uint
Model::GetType() const
{
	return m_type;
}

} // namespace gfx
