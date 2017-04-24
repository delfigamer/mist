#pragma once

#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>
#include <utility>
#include <cstddef>
#include <cstring>

class StringBuilder
{
private:
	class Node: public RefObject
	{
	private:
		Ref< Node > const m_prefix;
		Ref< DataBuffer > const m_center;
		Ref< Node > const m_suffix;

	public:
		template< typename PT, typename CT, typename ST >
		Node( PT&& prefix, CT&& center, ST&& suffix );
		size_t getlength();
		uint8_t* write( uint8_t* buffer );
	};

private:
	Ref< Node > m_node;

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
	uint8_t* write( uint8_t* buffer ) const;
	Ref< DataBuffer > combine() const;
	StringBuilder& operator<<( StringBuilder const& sb );
	StringBuilder& operator<<( StringBuilder&& sb );
	StringBuilder& operator<<( Ref< DataBuffer > const& db );
	StringBuilder& operator<<( Ref< DataBuffer >&& db );
	operator bool() const;
};

template< typename PT, typename CT, typename ST >
StringBuilder::Node::Node( PT&& prefix, CT&& center, ST&& suffix )
	: m_prefix( std::forward< PT >( prefix ) )
	, m_center( std::forward< CT >( center ) )
	, m_suffix( std::forward< ST >( suffix ) )
{
}

inline size_t StringBuilder::Node::getlength()
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

inline uint8_t* StringBuilder::Node::write( uint8_t* buffer )
{
	if( m_prefix )
	{
		buffer = m_prefix->write( buffer );
	}
	if( m_center )
	{
		size_t clen = m_center->m_length;
		memcpy( buffer, m_center->m_data, clen );
		buffer += clen;
	}
	if( m_suffix )
	{
		buffer = m_suffix->write( buffer );
	}
	return buffer;
}

inline StringBuilder::StringBuilder( std::nullptr_t )
{
}

inline StringBuilder::StringBuilder( StringBuilder const& other )
	: m_node( other.m_node )
{
}

inline StringBuilder::StringBuilder( StringBuilder&& other )
	: m_node( other.m_node )
{
}
inline StringBuilder::StringBuilder( Ref< DataBuffer > const& db )
{
	m_node.emplace( nullptr, db, nullptr );
}

inline StringBuilder::StringBuilder( Ref< DataBuffer >&& db )
{
	m_node.emplace( nullptr, std::move( db ), nullptr );
}

inline StringBuilder::~StringBuilder()
{
}

inline StringBuilder& StringBuilder::operator=( StringBuilder const& other )
{
	m_node = other.m_node;
	return *this;
}

inline StringBuilder& StringBuilder::operator=( StringBuilder&& other )
{
	m_node = std::move( other.m_node );
	return *this;
}

inline size_t StringBuilder::getlength() const
{
	if( m_node )
	{
		return m_node->getlength();
	}
	else
	{
		return 0;
	}
}

inline uint8_t* StringBuilder::write( uint8_t* buffer ) const
{
	if( m_node )
	{
		return m_node->write( buffer );
	}
	else
	{
		return buffer;
	}
}

inline Ref< DataBuffer > StringBuilder::combine() const
{
	if( m_node )
	{
		size_t len = m_node->getlength();
		Ref< DataBuffer > buffer = DataBuffer::create( len );
		m_node->write( buffer->m_data );
		return buffer;
	}
	else
	{
		return DataBuffer::create();
	}
}

inline StringBuilder& StringBuilder::operator<<( StringBuilder const& sb )
{
	if( sb.m_node )
	{
		if( m_node )
		{
			m_node = Ref< StringBuilder::Node >::create(
				std::move( m_node ), nullptr, sb.m_node );
		}
		else
		{
			m_node = sb.m_node;
		}
	}
	return *this;
}

inline StringBuilder& StringBuilder::operator<<( StringBuilder&& sb )
{
	if( sb.m_node )
	{
		if( m_node )
		{
			m_node = Ref< StringBuilder::Node >::create(
				std::move( m_node ), nullptr, std::move( sb.m_node ) );
		}
		else
		{
			m_node = std::move( sb.m_node );
		}
	}
	return *this;
}

inline StringBuilder& StringBuilder::operator<<( Ref< DataBuffer > const& db )
{
	if( db && db->m_length != 0 )
	{
		m_node = Ref< StringBuilder::Node >::create(
			std::move( m_node ), db, nullptr );
	}
	return *this;
}

inline StringBuilder& StringBuilder::operator<<( Ref< DataBuffer >&& db )
{
	if( db && db->m_length != 0 )
	{
		m_node = Ref< StringBuilder::Node >::create(
			std::move( m_node ), std::move( db ), nullptr );
	}
	return *this;
}

inline StringBuilder::operator bool() const
{
	return m_node;
}
