#pragma once 

namespace gk
{
/**
 * @class Noncopyable
 *
 * To make a class non-copyable, inherit from this.
 */
class Noncopyable
{
protected:
	Noncopyable() {}
	~Noncopyable() {}

private:  // emphasize the following members are private
	Noncopyable( const Noncopyable& );
	const Noncopyable& operator=( const Noncopyable& );
};
} // gk
