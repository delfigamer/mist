#pragma once

#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>
#include <utility>
#include <cinttypes>
#include <cstring>

class StringBuilder: public RefObject
{
private:
	Ref< StringBuilder > const m_prefix;
	Ref< DataBuffer > const m_center;
	Ref< StringBuilder > const m_suffix;

public:
	StringBuilder( Ref< StringBuilder > const& prefix,
		Ref< DataBuffer > const& center, Ref< StringBuilder > const& suffix );
	size_t getlength();
	size_t write( uint8_t* buffer, size_t bufsize );

	template< typename F, typename S, typename... T >
	static Ref< StringBuilder > construct(
		F&& first, S&& sedond, T&&... args );
	static Ref< StringBuilder > construct( char const* str );
	static Ref< StringBuilder > construct( DataBuffer* str );
	static Ref< StringBuilder > construct( StringBuilder* sb );
	static Ref< StringBuilder > construct();
};

inline StringBuilder::StringBuilder( Ref< StringBuilder > const& prefix,
	Ref< DataBuffer > const& center, Ref< StringBuilder > const& suffix )
	: m_prefix( prefix )
	, m_center( center )
	, m_suffix( suffix )
{
}

inline size_t StringBuilder::getlength()
{
	size_t length = 0;
	if( m_prefix )
	{
		length += m_prefix->getlength();
	}
	if( m_center )
	{
		length += m_center->m_length;
	}
	if( m_suffix )
	{
		length += m_suffix->getlength();
	}
	return length;
}

inline size_t StringBuilder::write( uint8_t* buffer, size_t bufsize )
{
	size_t length = 0;
	if( m_prefix && bufsize > 0 )
	{
		size_t res = m_prefix->write( buffer, bufsize );
		length += res;
		buffer += res;
		bufsize -= res;
	}
	if( m_center )
	{
		size_t clen = m_center->m_length;
		if( clen > bufsize )
		{
			clen = bufsize;
		}
		if( clen > 0 )
		{
			memcpy( buffer, m_center->m_data, clen );
			length += clen;
			buffer += clen;
			bufsize -= clen;
		}
	}
	if( m_suffix && bufsize > 0 )
	{
		size_t res = m_suffix->write( buffer, bufsize );
		length += res;
	}
	return length;
}

template< typename F, typename S, typename... T >
Ref< StringBuilder > StringBuilder::construct(
	F&& first, S&& second, T&&... args )
{
	auto left = construct( std::forward< F >( first ) );
	auto right = construct(
		std::forward< S >( second ), std::forward< T >( args )... );
	return Ref< StringBuilder >::create(
		left, nullptr, right );
}

inline Ref< StringBuilder > StringBuilder::construct( char const* str )
{
	size_t clen = strlen( str );
	return Ref< StringBuilder >::create(
		nullptr, DataBuffer::create( clen, clen, str ), nullptr );
}

inline Ref< StringBuilder > StringBuilder::construct( DataBuffer* str )
{
	return Ref< StringBuilder >::create(
		nullptr, str, nullptr );
}

inline Ref< StringBuilder > StringBuilder::construct( StringBuilder* sb )
{
	return Ref< StringBuilder >( sb );
}

inline Ref< StringBuilder > StringBuilder::construct()
{
	return Ref< StringBuilder >::create(
		nullptr, nullptr, nullptr );
}
