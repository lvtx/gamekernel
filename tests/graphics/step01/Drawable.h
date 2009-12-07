#pragma once

namespace gfx {

class Renderer;

/**
 * @class Drawable 
 */
class Drawable
{
public:
	Drawable();
	virtual ~Drawable();

	virtual void Draw( Renderer* renderer );
};

} // namespace gfx
