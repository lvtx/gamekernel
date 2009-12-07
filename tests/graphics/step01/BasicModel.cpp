#include "stdafx.h"

#include "BasicModel.h"

#include "Effect.h"
#include "Renderer.h"
#include "Texture.h"
#include <kcore/xml/tinyxml.h>

using namespace gfx;

BasicModel::BasicModel()
{
	m_type = Model::MODEL_BASIC;
}

BasicModel::~BasicModel()
{
	Unload();
}

/// Drawable::Draw()
void BasicModel::Draw( Renderer* renderer )
{
	K_ASSERT( renderer != 0 );
	K_ASSERT( m_effect != 0 );
	K_ASSERT( m_texture != 0 );
	K_ASSERT( !m_tries.empty() );
	K_ASSERT( !m_faces.empty() );

	if ( m_texture != 0 )
	{
		m_effect->SetTexture( "diffuse0", m_texture );
	}

	m_effect->SetTechnique( "t0" );

	// TODO: draw meshes grouped in effects
	
	uint passes = 0;

	m_effect->Begin( &passes );

	for ( uint i=0; i<passes; ++i )
	{
		m_effect->BeginPass( i );

		renderer->SetStreamSource( m_vbuf );
		renderer->SetIndices( m_ibuf );

		renderer->DrawIndexedPrimitive( 0, 
										0, 
										m_vbuf->GetTriangleCount(), 
										0, 
										m_ibuf->GetFaceCount() );

		m_effect->EndPass();
	}

	m_effect->End();
}

/// Model {
bool BasicModel::Load( Renderer* renderer, const std::string& file )
{
	K_ASSERT( renderer != 0 );

	bool rc = loadText( file );
	if ( !rc ) return false; 

	rc = createEffect( renderer );
	if ( !rc ) return false; 

	rc = createTexture( renderer );
	if ( !rc ) return false; 

	rc = createVertexBuffer( renderer );
	if ( !rc ) return false; 

	rc = createIndexBuffer( renderer );
	if ( !rc ) return false; 

	return true;
}

void BasicModel::Unload()
{
	K_DELETE( m_vbuf );
	K_DELETE( m_ibuf );
	K_DELETE( m_texture );
	K_DELETE( m_effect );
}

bool BasicModel::loadText( const std::string& file )
{
	XmlDocument doc; 

	bool rc = doc.LoadFile( file.c_str() );
	if ( !rc ) return false; 

	TiXmlElement* xmodel = doc.FirstChildElement( "model" );
	
	return loadModel( xmodel );
}

bool BasicModel::createEffect( Renderer* renderer )
{
	m_effect = renderer->CreateEffect( m_shader );

	return ( m_effect != 0 );
}

bool BasicModel::createTexture( Renderer* renderer )
{
	m_texture = renderer->CreateTexture( m_tex0 );

	return ( m_texure != 0 );
}

bool BasicModel::createVertexBuffer( Renderer* renderer )
{
	if ( m_tries.empty() ) return false; 

	m_vbuf = renderer->CreateVertexBuffer( m_tries.size(), 
										   TriMesh::STRIDE, 
										   TriMesh::FVF );

	if ( m_vbuf != 0 )
	{
		m_vbuf->Update( &m_tries[0], m_tries.size(), TriMesh::STRIDE );
	}

	return ( m_vbuf != 0 );
}

bool BasicModel::createIndexBuffer( Renderer* renderer )
{
	if ( m_faces.empty() ) return false; 

	m_ibuf = renderer->CreateIndexBuffer( m_faces.size() );

	if ( m_vbuf != 0 )
	{
		m_ibuf->Update( &m_faces[0], m_faces.size() ); 
	}

	return ( m_ibuf != 0 );
}

bool BasicModel::loadModel( TiXmlElement* xmodel )
{
	if ( xmodel == 0 ) return false; 

	const char* xtype = xmodel->Atrribute( "type" );
	if ( xtype == 0 ) return false; 
	if ( strcmp( xtype, "basic" ) != 0 ) return false;

	xshader = xmodel->FirstChildElement( "shader" );
	K_RETURN_V_IF_NOT( xshader != 0, false );
	K_RETURN_V_IF_NOT( loadShader( xshader ), false );

	xtexture = xmodel->FirstChildElement( "tex0" );
	K_RETURN_V_IF_NOT( xtexture != 0, false );
	K_RETURN_V_IF_NOT( loadTex( xtexture ), false );

	TiXmlElement* xv = xmodel->FirstChildElement( "v" );

	while ( xv != 0 )
	{
		loadVertex( xv );

		xv = xv->NextSiblingElement( "v" );
	}

	TiXmlElement* xf = xmodel->FirstChildElement( "f" );

	while ( xf != 0 )
	{
		loadFace( xf );

		xf = xf->NextSiblingElement( "f" );
	}

	return !m_tries.empty() && !m_faces.empty();
}

void BasicModel::loadVertex( TiXmlElement* xv )
{
	// <v x="1" y="2" z="3" c="31341341" nx="0.1" ny="0.1" nz="0.1 tx="0.0" ty="0.0""/>

}

void BasicModel::loadFace( TiXmlElement* xf )
{
	// <f x="1" y="1" z="1"/>

}



