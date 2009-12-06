#include "stdafx.h"

#include "Renderer.h"

Renderer::Renderer()
: m_params()
, m_error( 0 )
, m_effect( 0 )
, m_mesh( 0 )
{
}

Renderer::~Renderer()
{
}

bool Renderer::Init( const Params& params )
{
}

void Renderer::SetEffect( Effect* effect )
{
}

void Renderer::SetMesh( TriMesh* mesh )
{
}

void Renderer::Begin()
{
}

void Renderer::Render()
{
}

void Renderer::End()
{
}

void Renderer::OnLost()
{
}

void Renderer::OnRestored()
{
}

void Renderer::Fini()
{
}