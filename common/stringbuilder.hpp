#pragma once

#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <utility>
#include <deque>
#include <cstddef>
#include <cstring>

// use a template with a dummy argument to let the linker merge the multiple
// definitions from each inclusion of this header
template< typename T >
class StringBuilderBase
{
private:
	std::deque< Ref< DataBuffer > > m_parts;

public:
	StringBuilderBase( std::nullptr_t = nullptr );
	StringBuilderBase( StringBuilderBase< T > const& other );
	StringBuilderBase( StringBuilderBase< T >&& other );
	StringBuilderBase( Ref< DataBuffer > const& db );
	StringBuilderBase( Ref< DataBuffer >&& db );
	~StringBuilderBase();
	StringBuilderBase< T >& operator=( StringBuilderBase< T > const& other );
	StringBuilderBase< T >& operator=( StringBuilderBase< T >&& other );

	size_t getlength() const;
	Ref< DataBuffer > combine() const;
	StringBuilderBase< T >& append( StringBuilderBase< T > const& sb );
	StringBuilderBase< T >& append( StringBuilderBase< T >&& sb );
	StringBuilderBase< T >& append( Ref< DataBuffer > const& db );
	StringBuilderBase< T >& append( Ref< DataBuffer >&& db );
	StringBuilderBase< T >& prepend( StringBuilderBase< T > const& sb );
	StringBuilderBase< T >& prepend( StringBuilderBase< T >&& sb );
	StringBuilderBase< T >& prepend( Ref< DataBuffer > const& db );
	StringBuilderBase< T >& prepend( Ref< DataBuffer >&& db );
	StringBuilderBase< T >& padleft( size_t targetlength, uint8_t ch = ' ' );
	StringBuilderBase< T >& padright( size_t targetlength, uint8_t ch = ' ' );
	operator bool() const;
};

typedef StringBuilderBase< void > StringBuilder;

template< typename T >
StringBuilderBase< T >::StringBuilderBase( std::nullptr_t )
{
}

template< typename T >
StringBuilderBase< T >::StringBuilderBase( StringBuilderBase< T > const& other )
	: m_parts( other.m_parts )
{
}

template< typename T >
StringBuilderBase< T >::StringBuilderBase( StringBuilderBase< T >&& other )
	: m_parts( std::move( other.m_parts ) )
{
}

template< typename T >
StringBuilderBase< T >::StringBuilderBase( Ref< DataBuffer > const& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.emplace_back( db );
	}
}

template< typename T >
StringBuilderBase< T >::StringBuilderBase( Ref< DataBuffer >&& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.emplace_back( std::move( db ) );
	}
}

template< typename T >
StringBuilderBase< T >::~StringBuilderBase()
{
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::operator=(
	StringBuilderBase< T > const& other )
{
	m_parts = other.m_parts;
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::operator=(
	StringBuilderBase< T >&& other )
{
	m_parts = std::move( other.m_parts );
	return *this;
}

template< typename T >
size_t StringBuilderBase< T >::getlength() const
{
	size_t length = 0;
	for( auto& part : m_parts )
	{
		length += part->m_length;
	}
	return length;
}

template< typename T >
Ref< DataBuffer > StringBuilderBase< T >::combine() const
{
	auto db = DataBuffer::create( getlength() );
	uint8_t* ptr = db->m_data;
	for( auto& part : m_parts )
	{
		memcpy( ptr, part->m_data, part->m_length );
		ptr += part->m_length;
	}
	return db;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::append(
	StringBuilderBase< T > const& sb )
{
	m_parts.insert( m_parts.end(), sb.m_parts.begin(), sb.m_parts.end() );
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::append(
	StringBuilderBase< T >&& sb )
{
	while( !sb.m_parts.empty() )
	{
		m_parts.push_back( std::move( sb.m_parts.front() ) );
		sb.m_parts.pop_front();
	}
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::append(
	Ref< DataBuffer > const& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.push_back( db );
	}
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::append(
	Ref< DataBuffer >&& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.push_back( std::move( db ) );
	}
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::prepend(
	StringBuilderBase< T > const& sb )
{
	m_parts.insert( m_parts.begin(), sb.m_parts.begin(), sb.m_parts.end() );
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::prepend(
	StringBuilderBase< T >&& sb )
{
	while( !sb.m_parts.empty() )
	{
		m_parts.push_front( std::move( sb.m_parts.back() ) );
		sb.m_parts.pop_back();
	}
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::prepend(
	Ref< DataBuffer > const& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.push_front( db );
	}
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::prepend(
	Ref< DataBuffer >&& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.push_front( std::move( db ) );
	}
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::padleft(
	size_t targetlength, uint8_t ch )
{
	size_t length = getlength();
	if( length < targetlength )
	{
		auto db = DataBuffer::create( targetlength - length );
		memset( db->m_data, ch, db->m_length );
		prepend( std::move( db ) );
	}
	return *this;
}

template< typename T >
StringBuilderBase< T >& StringBuilderBase< T >::padright(
	size_t targetlength, uint8_t ch )
{
	size_t length = getlength();
	if( length < targetlength )
	{
		auto db = DataBuffer::create( targetlength - length );
		memset( db->m_data, ch, db->m_length );
		append( std::move( db ) );
	}
	return *this;
}

template< typename T >
StringBuilderBase< T >::operator bool() const
{
	return !m_parts.empty();
}

template< typename T >
inline StringBuilder& operator+=(
	StringBuilder& a, T&& b )
{
	a.append( std::forward< T >( b ) );
	return a;
}

inline StringBuilder operator+(
	StringBuilder const& a, StringBuilder const& b )
{
	return std::move( StringBuilder( a ).append( b ) );
}

inline StringBuilder operator+(
	StringBuilder const& a, StringBuilder&& b )
{
	return std::move( b.prepend( a ) );
}

inline StringBuilder operator+(
	StringBuilder const& a, Ref< DataBuffer > const& b )
{
	return std::move( StringBuilder( a ).append( b ) );
}

inline StringBuilder operator+(
	StringBuilder const& a, Ref< DataBuffer >&& b )
{
	return std::move( StringBuilder( a ).append( std::move( b ) ) );
}

inline StringBuilder operator+(
	StringBuilder&& a, StringBuilder const& b )
{
	return std::move( a.append( b ) );
}

inline StringBuilder operator+(
	StringBuilder&& a, StringBuilder&& b )
{
	return std::move( a.append( std::move( b ) ) );
}

inline StringBuilder operator+(
	StringBuilder&& a, Ref< DataBuffer > const& b )
{
	return std::move( a.append( b ) );
}

inline StringBuilder operator+(
	StringBuilder&& a, Ref< DataBuffer >&& b )
{
	return std::move( a.append( std::move( b ) ) );
}

inline StringBuilder operator+(
	Ref< DataBuffer > const& a, StringBuilder const& b )
{
	return std::move( StringBuilder( a ).append( b ) );
}

inline StringBuilder operator+(
	Ref< DataBuffer > const& a, StringBuilder&& b )
{
	return std::move( b.prepend( a ) );
}

inline StringBuilder operator+(
	Ref< DataBuffer > const& a, Ref< DataBuffer > const& b )
{
	return std::move( StringBuilder( a ).append( b ) );
}

inline StringBuilder operator+(
	Ref< DataBuffer > const& a, Ref< DataBuffer >&& b )
{
	return std::move( StringBuilder( a ).append( std::move( b ) ) );
}

inline StringBuilder operator+(
	Ref< DataBuffer >&& a, StringBuilder const& b )
{
	return std::move( StringBuilder( std::move( a ) ).append( b ) );
}

inline StringBuilder operator+(
	Ref< DataBuffer >&& a, StringBuilder&& b )
{
	return std::move( b.prepend( std::move( a ) ) );
}

inline StringBuilder operator+(
	Ref< DataBuffer >&& a, Ref< DataBuffer > const& b )
{
	return std::move( StringBuilder( std::move( a ) ).append( b ) );
}

inline StringBuilder operator+(
	Ref< DataBuffer >&& a, Ref< DataBuffer >&& b )
{
	return std::move(
		StringBuilder( std::move( a ) ).append( std::move( b ) ) );
}
