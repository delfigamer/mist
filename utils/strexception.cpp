#include "strexception.hpp"
#include "ref.hpp"
#include <cstdio>
#include <cstdarg>

namespace utils {
	StrException::StrException()
	{
	}

	StrException::StrException( char const* format, ... )
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

	StrException::StrException( utils::String const& message )
		: m_message( message )
	{
	}

	StrException::StrException( StrException const& other )
		: m_message( other.m_message )
	{
	}

	StrException::StrException( StrException&& other )
		: m_message( std::move( other.m_message ) )
	{
	}

	StrException& StrException::operator=( StrException const& other )
	{
		m_message = other.m_message;
		return *this;
	}

	StrException& StrException::operator=( StrException&& other )
	{
		m_message = std::move( other.m_message );
		return *this;
	}

	char const* StrException::what() const noexcept
	{
		return m_message;
	}
}