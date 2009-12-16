#include "stdafx.h"

#include "Model.h"

#include "Renderer.h"

using namespace gfx; 

Model::Model()
{
}

Model::~Model()
{
}

bool Model::Load( Renderer* renderer, const std::string& file )
{
	renderer;
	file;

	return false; 
}

void Model::Unload()
{
	// empty
}
