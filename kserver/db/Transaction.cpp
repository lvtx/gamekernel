#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/db/Transaction.h>

namespace gk 
{
Transaction::Transaction( uint type )
: m_type( type )
, m_cellId()
, m_txNo( 0 )
, m_result( SUCCESS )
{
}

Transaction::~Transaction()
{
}

} // gk
