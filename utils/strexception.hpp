#pragma once

#include <utils/string.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <exception>
#include <cstdio>
#include <cstdarg>

namespace utils
{
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
		int bufsize = vsnprintf( 0, 0, format, vl );
		va_end( vl );
		va_start( vl, format );
		Ref< DataBuffer > buffer = DataBuffer::create( 0, bufsize + 1, 0 );
		buffer->m_length = 1 + vsnprintf(
			( char* )buffer->m_data, buffer->m_capacity, format, vl );
		va_end( vl );
		m_message.m_payload = buffer;
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
}
