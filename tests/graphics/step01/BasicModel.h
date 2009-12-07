#pragma once 

#include "TriMesh.h"
#include "Model.h"

namespace gfx {

class Effect; 
class Renderer;
class Texture; 
class TiXmlElement;

/**
 * @class BasicModel 
 *
 * bmt, bmb (text and binary format)
 * bmt uses XML to define mesh model.
 *
 * TODO: Make this work as a Resource
 * TODO: Figure out better interfaces for Mesh models
 */
class BasicModel : public Model
{
public:
	BasicModel();
	virtual ~BasicModel();

	/// Drawable::Draw
	void Draw( Renderer* renderer );

	/// Model {
	bool Load( Renderer* renderer, const std::string& file );
	void Unload();
	/// }
	
	const std::string& GetTex0() const;
	const std::string& GetShader() const;

protected:
	typedef std::vector<TriMesh> TriList;
	typedef std::vector<short> FaceList;

	bool loadText( const std::string& file );
	bool createEffect( Renderer* renderer );
	bool createTexture( Renderer* renderer );
	bool createVertexBuffer( Renderer* renderer );
	bool createIndexBuffer( Renderer* renderer );

	bool loadModel( TiXmlElement* xmodel );

protected:
	std::string 		m_tex0;	 	// the first and only texture
	std::string 		m_shader;	// shader path for this model
	TriList 			m_tries; 	// temporal feature to test rendering
	FaceList 			m_faces; 	// temporal feature to test rendering 

	Effect* 			m_effect;  	// temporal feature to test rendering
	Texture* 			m_texture;  // temporal feature to test rendering
	VertexBuffer* 		m_vbuf;  	// temporal feature to test rendering
	IndexBuffer* 		m_ibuf;  	// temporal feature to test rendering
}; 

inline
const std::string& 
BasicModel::GetTex0() const
{
	return m_tex0;
}

inline
const std::string& 
BasicModel::GetShader() const
{
	return m_shader;
}

} // namespace gfx
