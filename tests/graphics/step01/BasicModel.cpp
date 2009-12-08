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

	m_effect->Begin( passes );

	for ( uint i=0; i<passes; ++i )
	{
		m_effect->BeginPass( i );

		renderer->SetStreamSource( m_vbuf );
		renderer->SetIndices( m_ibuf );

		renderer->DrawIndexed( 0, 
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
	TiXmlDocument doc; 

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

	return ( m_texture != 0 );
}

bool BasicModel::createVertexBuffer( Renderer* renderer )
{
	if ( m_tries.empty() ) return false; 

	m_vbuf = renderer->CreateVertexBuffer( m_tries.size(), 
										   TriMesh::STRIDE, 
										   TriMesh::FVF );

	if ( m_vbuf == 0 ) 
	{
		return false; 
	}

	m_vbuf->Update( (byte*)(&m_tries[0]), m_tries.size(), TriMesh::STRIDE );

	return true;
}

bool BasicModel::createIndexBuffer( Renderer* renderer )
{
	if ( m_faces.empty() ) return false; 

	m_ibuf = renderer->CreateIndexBuffer( m_faces.size() );

	if ( m_vbuf == 0 )
	{
		return false;
	}

	m_ibuf->Update( (byte*)(&m_faces[0]), m_faces.size() ); 

	return true;
}

bool BasicModel::loadModel( TiXmlElement* xmodel )
{
	if ( xmodel == 0 ) return false; 

	const char* xtype = xmodel->Attribute( "type" );
	if ( xtype == 0 ) return false; 
	if ( strcmp( xtype, "basic" ) != 0 ) return false;

	TiXmlElement* xshader = xmodel->FirstChildElement( "shader" );
	K_RETURN_V_IF( xshader == 0, false );
	K_RETURN_V_IF( loadShader( xshader ) == false, false );

	TiXmlElement* xtexture = xmodel->FirstChildElement( "tex0" );
	K_RETURN_V_IF( xtexture == 0, false );
	K_RETURN_V_IF( loadTexture( xtexture ) == false, false );

	TiXmlElement* xvertices = xmodel->FirstChildElement( "vertices" );
	K_RETURN_V_IF( xvertices == 0, false );

	TiXmlElement* xv = xvertices->FirstChildElement( "v" );

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

bool BasicModel::loadShader( TiXmlElement* xshader )
{
	const char* ashader = xshader->Attribute( "path" );
	K_RETURN_V_IF( ashader == 0, false );

	m_shader = ashader;	

	return true;
}

bool BasicModel::loadTexture( TiXmlElement* xtexture )
{
	const char* atex = xtexture->Attribute( "path" );
	K_RETURN_V_IF( atex == 0, false );

	m_tex0 = atex;

	return true;
}

void BasicModel::loadVertex( TiXmlElement* xv )
{
	// <v x="1" y="2" z="3" c="31341341" nx="0.1" ny="0.1" nz="0.1 tx="0.0" ty="0.0""/>
	TriMesh m;
	memset( (void*)&m, 0, sizeof(TriMesh) );

	xv->Attribute( "x", (double*)&m.x );
	xv->Attribute( "y", (double*)&m.y );
	xv->Attribute( "z", (double*)&m.z );

	xv->Attribute( "c", (double*)&m.color );

	xv->Attribute( "nx", (double*)&m.nx );
	xv->Attribute( "ny", (double*)&m.ny );
	xv->Attribute( "nz", (double*)&m.nz );

	xv->Attribute( "tx", (double*)&m.tex1.u );
	xv->Attribute( "ty", (double*)&m.tex1.v );

	m_tries.push_back( m );
}

void BasicModel::loadFace( TiXmlElement* xf )
{
	// <f x="1" y="1" z="1"/>
	
	short x = 0;
	short y = 0;
	short z = 0;

	xf->Attribute( "x", (double*)&x );
	xf->Attribute( "y", (double*)&y );
	xf->Attribute( "z", (double*)&z );

	m_faces.push_back( x );
	m_faces.push_back( y );
	m_faces.push_back( z );
}

