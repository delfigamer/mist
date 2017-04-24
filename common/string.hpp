#pragma once

#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <atomic>
#include <utility>
#include <cstdarg>
#include <cstdio>
#include <cstring>

class String
{
public:
	Ref< DataBuffer > m_payload;

public:
	String();
	String( const char* newchars, size_t length = 0 );
	String( const String& other ) NOEXCEPT;
	String( String&& other ) NOEXCEPT;
	String( Ref< DataBuffer > const& db );
	String( Ref< DataBuffer >&& db );
	~String() NOEXCEPT;
	String& operator=( const String& other ) NOEXCEPT;
	String& operator=( String&& other ) NOEXCEPT;

	operator bool() const NOEXCEPT;
	operator char const*() const NOEXCEPT;
	size_t getlength() const NOEXCEPT;
	const char* getchars() const NOEXCEPT;
	void setchars( const char* newchars, size_t length = 0 );

	ATTRIBUTE(( __format__( gnu_printf, 1, 0 ) ))
	static String vformat( const char* fmt, va_list vl );
	ATTRIBUTE(( __format__( gnu_printf, 1, 2 ) ))
	static String format( const char* fmt, ... );
};

inline String::String()
	: m_payload( nullptr )
{
}

inline String::String( const char* newchars, size_t length )
	: m_payload( nullptr )
{
	setchars( newchars, length );
}

inline String::String( const String& other ) NOEXCEPT
	: m_payload( other.m_payload )
{
}

inline String::String( String&& other ) NOEXCEPT
	: m_payload( std::move( other.m_payload ) )
{
}

inline String::String( Ref< DataBuffer > const& db )
	: m_payload( db )
{
}

inline String::String( Ref< DataBuffer >&& db )
	: m_payload( std::move( db ) )
{
}

inline String::~String() NOEXCEPT
{
}

inline String& String::operator=( const String& other ) NOEXCEPT
{
	m_payload = other.m_payload;
	return *this;
}

inline String& String::operator=( String&& other ) NOEXCEPT
{
	m_payload = std::move( other.m_payload );
	return *this;
}

inline String::operator bool() const NOEXCEPT
{
	return m_payload;
}

inline String::operator char const*() const NOEXCEPT
{
	return getchars();
}

inline size_t String::getlength() const NOEXCEPT
{
	DataBuffer* e = m_payload;
	return ( e ? e->m_length - 1 : 0 );
}

inline const char* String::getchars() const NOEXCEPT
{
	DataBuffer* e = m_payload;
	std::atomic_thread_fence( std::memory_order_acquire );
	return ( e ? ( char const* )e->m_data : "" );
}

inline void String::setchars( const char* newchars, size_t length )
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
	Ref< DataBuffer > payload = DataBuffer::create(
		length, length + 1, newchars );
	payload->m_data[ length ] = 0;
	payload->m_length = length + 1;
	std::atomic_thread_fence( std::memory_order_release );
	m_payload = payload;
}

inline String String::vformat( char const* fmt, va_list vl )
{
	va_list vl2;
	va_copy( vl2, vl );
	size_t bufsize = vsnprintf( 0, 0, fmt, vl2 );
	va_end( vl2 );
	Ref< DataBuffer > buffer = DataBuffer::create( 0, bufsize + 1, 0 );
	buffer->m_length = 1 + vsnprintf(
		( char* )buffer->m_data, buffer->m_capacity, fmt, vl );
	return String( buffer );
}

inline String String::format( char const* fmt, ... )
{
	va_list vl;
	va_start( vl, fmt );
	String result = vformat( fmt, vl );
	va_end( vl );
	return result;
}
