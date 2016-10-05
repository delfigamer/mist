#pragma once

#include <common/string.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <exception>
#include <cstdio>
#include <cstdarg>

class StrException: public std::exception
{
private:
	String m_message;

public:
	StrException();
	StrException( char const* format, ... );
	StrException( String const& message );
	StrException( StrException const& other );
	StrException( StrException&& other );
	StrException& operator=( StrException const& other );
	StrException& operator=( StrException&& other );
	virtual char const* what() const NOEXCEPT override;
};

inline StrException::StrException()
{
}

inline StrException::StrException( char const* format, ... )
{
	va_list vl;
	va_start( vl, format );
	m_message = String::format( format, vl );
	va_end( vl );
}

inline StrException::StrException( String const& message )
	: m_message( message )
{
}

inline StrException::StrException( StrException const& other )
	: m_message( other.m_message )
{
}

inline StrException::StrException( StrException&& other )
	: m_message( std::move( other.m_message ) )
{
}

inline StrException& StrException::operator=( StrException const& other )
{
	m_message = other.m_message;
	return *this;
}

inline StrException& StrException::operator=( StrException&& other )
{
	m_message = std::move( other.m_message );
	return *this;
}

inline char const* StrException::what() const NOEXCEPT
{
	return m_message;
}
