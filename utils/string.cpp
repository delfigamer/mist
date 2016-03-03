#include "string.hpp"
#include <utility>
#include <cstring>

namespace utils
{
	String::String()
		: m_payload( nullptr )
	{
	}

	String::String( const char* newchars, int length )
		: m_payload( nullptr )
	{
		setchars( newchars, length );
	}

	String::String( const String& other ) NOEXCEPT
		: m_payload( other.m_payload )
	{
	}

	String::String( String&& other ) NOEXCEPT
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

	String::~String() NOEXCEPT
	{
	}

	String& String::operator=( const String& other ) NOEXCEPT
	{
		m_payload = other.m_payload;
		return *this;
	}

	String& String::operator=( String&& other ) NOEXCEPT
	{
		m_payload = std::move( other.m_payload );
		return *this;
	}

	String::operator bool() const NOEXCEPT
	{
		return m_payload != 0;
	}

	String::operator char const*() const NOEXCEPT
	{
		return getchars();
	}

	int String::getlength() const NOEXCEPT
	{
		DataBuffer* e = m_payload;
		return ( e ? e->m_length : 0 );
	}

	const char* String::getchars() const NOEXCEPT
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
