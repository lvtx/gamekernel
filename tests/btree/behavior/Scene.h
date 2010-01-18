#pragma once 

namespace bt 
{

/**
 * @class Scene 
 * 
 * 그냥 장면 하나가 있는 것처럼 한다. 
 * 간략하게 Entity들을 인스턴스 이름으로 보관한다. 
 */
class Scene 
{
public:
	Scene();
	~Scene();

	void Add( Entity* e );

	Entity* Find( const tstring& name );

	void Remove( const tstring& name );

private:
	typedef stdext::hash_map<tstring, Entity*> EntityMap;

	EntityMap m_entities;
};

} // namespace bt 
