#pragma once 

namespace gk {

/**
 * @class DbConnection 
 *
 * Base class for DbConnection. 
 * Each DbPool has one connection type. 
 */
class DbConnection 
{
public:
	virtual ~DbConnection() {}
};

/**
 * @class DbPool 
 *
 * @brief DbPool interface for connection pooling
 */
class DbPool 
{
public:
	DbPool() {}
	virtual ~DbPool() {}

	/**
	 * @brief Initialize 
	 * 
	 * @param cfg The configuration file 
	 * @return true if successful
	 */
	virtual bool Init( const tstring& cfg ) = 0;

	/**
	 * @brief Acquire db connection 
	 *
	 * @param db The database name to get connection 
	 * @return DbConnection. 0 if it cannot be acquired.
	 */
	virtual DbConnection* Acquire( const tstring& db ) = 0;

	/**
	 * @brief Release a db connection to the pool 
	 *
	 * @param db The database name 
	 * @param c The database connection 
	 */
	virtual void Release( const tstring& db, DbConnection* c ) = 0;

	/**
	 * @brief Check database connections
	 */
	virtual void Check() = 0;

	/**
	 * @brief Clean up the whole pool 
	 */
	virtual void Fini() = 0;
};

/**
 * @class DbAutoPtr
 *
 * @brief Auto ptr for Db connection
 */
class DbAutoPtr
{
public:
	DbAutoPtr( DbPool* pool = 0, const tstring& db = _T("") )
		: m_pool( pool )
		, m_ptr( 0 )
		, m_db( db )
	{
		if ( m_pool != 0 )
		{
			m_ptr = m_pool->Acquire( db );

			K_ASSERT( m_ptr != 0 );
		}
	}

	~DbAutoPtr()
	{
		if ( m_pool != 0 )
		{
			if ( m_ptr != 0 )
			{
				m_pool->Release( m_db, m_ptr );
			}
		}

		m_pool = 0;
		m_ptr  = 0;
	}

	DbConnection* Get()
	{
		K_ASSERT( m_pool != 0 );
		K_ASSERT( m_ptr != 0 );

		return m_ptr;
	}

	DbAutoPtr( const DbAutoPtr& rhs )
	{
		m_pool = rhs.m_pool;
		m_ptr  = rhs.m_ptr;
		m_db   = rhs.m_db;
	}

	DbAutoPtr& operator=( const DbAutoPtr& rhs )
	{
		m_pool = rhs.m_pool;
		m_ptr  = rhs.m_ptr;
		m_db   = rhs.m_db;

		return *this;
	}

private:
	DbPool*		  m_pool;
	DbConnection* m_ptr;
	tstring		  m_db;
};


} // gk
