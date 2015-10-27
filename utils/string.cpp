#include "string.hpp"
#include <utility>
#include <cstring>

namespace utils
{
#define finally( op ) catch(...) {{op} throw;} {op}

	String::String()
		: m_payload( nullptr )
	{
	}

	String::String( const char* newchars, int length )
		: m_payload( nullptr )
	{
		setchars( newchars, length );
	}

	String::String( const String& other ) noexcept
		: m_payload( other.m_payload )
	{
	}

	String::String( String&& other ) noexcept
		: m_payload( std::move( other.m_payload ) )
	{
	}

	String::String( Ref< DataBuffer > const& db )
		: m_payload( db )
	{
	}

	String::String( Ref< DataBuffer >&& db )
		: m_payload( std::move( db ) )
	{
	}

	String::~String() noexcept
	{
	}

	String& String::operator=( const String& other ) noexcept
	{
		m_payload = other.m_payload;
		return *this;
	}

	String& String::operator=( String&& other ) noexcept
	{
		m_payload = std::move( other.m_payload );
		return *this;
	}

	String::operator bool() const noexcept
	{
		return m_payload;
	}

	String::operator char const*() const noexcept
	{
		return getchars();
	}

	int String::getlength() const noexcept
	{
		DataBuffer* e = m_payload;
		return ( e ? e->m_length : 0 );
	}

	const char* String::getchars() const noexcept
	{
		DataBuffer* e = m_payload;
		return ( e ? ( char const* )e->m_data : "" );
	}

	void String::setchars( const char* newchars, int length )
	{
		if( !newchars )
		{
			m_payload = nullptr;
			return;
		}
		if( length == 0 )
		{
			length = strlen( newchars );
			if( length == 0 )
			{
				m_payload = nullptr;
				return;
			}
		}
		Ref< DataBuffer > payload = DataBuffer::create( length, length + 1, newchars );
		payload->m_data[ length ] = 0;
		payload->m_length = length + 1;
		m_payload = payload;
	}
}
