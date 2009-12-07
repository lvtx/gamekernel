#include "stdafx.h"

#include "Drawable.h"

using namespace gfx; 

Drawable::Drawable()
{
}

Drawable::~Drawable()
{
}

void Drawable::Draw( Renderer* renderer )
{
	K_ASSERT( renderer != 0 );

	// empty
}
