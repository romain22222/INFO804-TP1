#include <ostream>
#include "Utils.h"

// Utils.cpp
std::ostream& operator<<( std::ostream& out, Vecteur v )
{
	out << v[ 0 ] << " " << v[ 1 ] << " " << v[ 2 ];
	return out;
}
std::istream& operator>>( std::istream& in, Vecteur& v )
{
	in >> v[ 0 ] >> v[ 1 ] >> v[ 2 ];
	return in;
}

std::ostream& operator<<( std::ostream& out, Triangle t )
{
	out << t[ 0 ] << " " << t[ 1 ] << " " << t[ 2 ];
	return out;
}
std::istream& operator>>( std::istream& in, Triangle& t )
{
	in >> t[ 0 ] >> t[ 1 ] >> t[ 2 ];
	return in;
}

std::ostream& operator<<( std::ostream& out, Index i )
{
	out << i[0] << " " << i[1] << " " << i[2];
	return out;
}