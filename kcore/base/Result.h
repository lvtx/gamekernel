#pragma once

namespace gk 
{

struct Result 
{
	enum { SUCCESS, FAIL }; 

	int code;

	Result( int code = SUCCESS ) 
	{ 
		this->code = code; 
	}

	operator bool () const 
	{ 
		return code == SUCCESS; 
	}
};

} // namespace gk
