#pragma once

#include <string>

namespace gk 
{
/** 
 * @class PanicError
 *
 * @brief panic error exception 
 */
class PanicError
{
public:
    PanicError( const tstring& msg );
    ~PanicError();

    PanicError( const PanicError& rhs );
    const PanicError& operator=( const PanicError& rhs);

    const tstring& GetMessage() const;

private:
    tstring msg_;
};

inline
PanicError::PanicError( const tstring& msg )
: msg_( msg )
{
}

inline
PanicError::~PanicError()
{
}

inline 
PanicError::PanicError( const PanicError& rhs )
{
    msg_ = rhs.msg_;
}

inline 
const PanicError&
PanicError::operator=( const PanicError& rhs )
{
    msg_ = rhs.msg_;

    return *this;
}

inline
const tstring& 
PanicError::GetMessage() const
{
    return msg_;
}

} // gk
