#pragma once

#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <utility>
#include <deque>
#include <cstddef>
#include <cstring>

class StringBuilder
{
private:
	std::deque< Ref< DataBuffer > > m_parts;

public:
	StringBuilder( std::nullptr_t = nullptr );
	StringBuilder( StringBuilder const& other );
	StringBuilder( StringBuilder&& other );
	StringBuilder( Ref< DataBuffer > const& db );
	StringBuilder( Ref< DataBuffer >&& db );
	~StringBuilder();
	StringBuilder& operator=( StringBuilder const& other );
	StringBuilder& operator=( StringBuilder&& other );

	size_t getlength() const;
	Ref< DataBuffer > combine() const;
	StringBuilder& append( StringBuilder const& sb );
	StringBuilder& append( StringBuilder&& sb );
	StringBuilder& append( Ref< DataBuffer > const& db );
	StringBuilder& append( Ref< DataBuffer >&& db );
	StringBuilder& prepend( StringBuilder const& sb );
	StringBuilder& prepend( StringBuilder&& sb );
	StringBuilder& prepend( Ref< DataBuffer > const& db );
	StringBuilder& prepend( Ref< DataBuffer >&& db );
	StringBuilder& padleft( size_t targetlength, uint8_t ch = ' ' );
	StringBuilder& padright( size_t targetlength, uint8_t ch = ' ' );
	operator bool() const;
};

inline StringBuilder::StringBuilder( std::nullptr_t )
{
}

inline StringBuilder::StringBuilder( StringBuilder const& other )
	: m_parts( other.m_parts )
{
}

inline StringBuilder::StringBuilder( StringBuilder&& other )
	: m_parts( std::move( other.m_parts ) )
{
}

inline StringBuilder::StringBuilder( Ref< DataBuffer > const& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.emplace_back( db );
	}
}

inline StringBuilder::StringBuilder( Ref< DataBuffer >&& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.emplace_back( std::move( db ) );
	}
}

inline StringBuilder::~StringBuilder()
{
}

inline StringBuilder& StringBuilder::operator=(
	StringBuilder const& other )
{
	m_parts = other.m_parts;
	return *this;
}

inline StringBuilder& StringBuilder::operator=(
	StringBuilder&& other )
{
	m_parts = std::move( other.m_parts );
	return *this;
}

inline size_t StringBuilder::getlength() const
{
	size_t length = 0;
	for( auto& part : m_parts )
	{
		length += part->m_length;
	}
	return length;
}

inline Ref< DataBuffer > StringBuilder::combine() const
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

inline StringBuilder& StringBuilder::append(
	StringBuilder const& sb )
{
	m_parts.insert( m_parts.end(), sb.m_parts.begin(), sb.m_parts.end() );
	return *this;
}

inline StringBuilder& StringBuilder::append(
	StringBuilder&& sb )
{
	while( !sb.m_parts.empty() )
	{
		m_parts.push_back( std::move( sb.m_parts.front() ) );
		sb.m_parts.pop_front();
	}
	return *this;
}

inline StringBuilder& StringBuilder::append(
	Ref< DataBuffer > const& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.push_back( db );
	}
	return *this;
}

inline StringBuilder& StringBuilder::append(
	Ref< DataBuffer >&& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.push_back( std::move( db ) );
	}
	return *this;
}

inline StringBuilder& StringBuilder::prepend(
	StringBuilder const& sb )
{
	m_parts.insert( m_parts.begin(), sb.m_parts.begin(), sb.m_parts.end() );
	return *this;
}

inline StringBuilder& StringBuilder::prepend(
	StringBuilder&& sb )
{
	while( !sb.m_parts.empty() )
	{
		m_parts.push_front( std::move( sb.m_parts.back() ) );
		sb.m_parts.pop_back();
	}
	return *this;
}

inline StringBuilder& StringBuilder::prepend(
	Ref< DataBuffer > const& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.push_front( db );
	}
	return *this;
}

inline StringBuilder& StringBuilder::prepend(
	Ref< DataBuffer >&& db )
{
	if( db && db->m_length != 0 )
	{
		m_parts.push_front( std::move( db ) );
	}
	return *this;
}

inline StringBuilder& StringBuilder::padleft(
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

inline StringBuilder& StringBuilder::padright(
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

inline StringBuilder::operator bool() const
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
