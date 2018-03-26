#pragma once

#include <common/stringbuilder.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <exception>
#include <string>
#include <cstdio>
#include <cstdarg>

class StrException: public std::exception
{
private:
	std::string m_message;

public:
	StrException();
	ATTRIBUTE(( __format__( gnu_printf, 2, 3 ) ))
	StrException( char const* format, ... );
	StrException( std::string const& message );
	StrException( std::string&& message );
	StrException( StringBuilder const& message );
	StrException( StrException const& other ) = default;
	StrException( StrException&& other ) = default;
	StrException& operator=( StrException const& other ) = default;
	StrException& operator=( StrException&& other ) = default;
	virtual char const* what() const noexcept override;
};

inline StrException::StrException()
{
}

inline StrException::StrException( char const* format, ... )
{
	char buffer[ 1024 ];
	va_list vl;
	va_start( vl, format );
	vsnprintf( buffer, sizeof( buffer ), format, vl );
	va_end( vl );
	m_message = buffer;
}

inline StrException::StrException( std::string const& message )
	: m_message( message )
{
}

inline StrException::StrException( std::string&& message )
	: m_message( std::move( message ) )
{
}

inline StrException::StrException( StringBuilder const& message )
{
	Ref< DataBuffer > buf = message.combine();
	if( buf )
	{
		m_message = std::string( buf->m_data, buf->m_data + buf->m_length );
	}
}

inline char const* StrException::what() const noexcept
{
	return m_message.c_str();
}
