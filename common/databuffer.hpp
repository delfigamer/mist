#pragma once

#include <common/refobject.hpp>
#include <common/ref.hpp>
#include <common.hpp>

class [[ r::class, r::name( "databuffer" ) ]] DataBuffer: public RefObject
{
public:
	static void* operator new( size_t size, void* ptr );

public:
	[[ r::field, r::name( "length" ) ]] size_t m_length;
	[[ r::field, r::name( "capacity" ) ]] size_t const m_capacity;
	[[ r::field, r::name( "data" ) ]] uint8_t m_data[ 8 ];

public:
	[[ r::method, r::name( "create" ) ]] static DataBuffer* newinstance(
		size_t length, size_t capacity, void const* data );
	static Ref< DataBuffer > create(
		size_t length, size_t capacity, void const* data );
	static Ref< DataBuffer > create( size_t length, void const* data );
	static Ref< DataBuffer > create( size_t length );
	static Ref< DataBuffer > create();

private:
	DataBuffer(
		size_t length, size_t capacity, void const* data ) noexcept;

protected:
	~DataBuffer();
	virtual void destroy() override;

public:
	DataBuffer( DataBuffer const& ) = delete;
	DataBuffer& operator=( DataBuffer const& ) = delete;
};

inline void* DataBuffer::operator new( size_t size, void* ptr )
{
	return ptr;
}

inline DataBuffer* DataBuffer::newinstance(
	size_t length, size_t capacity, void const* data )
{
	DataBuffer* db;
	db = ( DataBuffer* )::operator new(
		offsetof( DataBuffer, m_data ) + capacity );
	new( db )DataBuffer( length, capacity, data );
	return db;
}

inline Ref< DataBuffer > DataBuffer::create(
	size_t length, size_t capacity, void const* data )
{
	return Ref< DataBuffer >( newinstance( length, capacity, data ), 0 );
}

inline Ref< DataBuffer > DataBuffer::create( size_t length, void const* data )
{
	return Ref< DataBuffer >( newinstance( length, length, data ), 0 );
}

inline Ref< DataBuffer > DataBuffer::create( size_t length )
{
	return Ref< DataBuffer >( newinstance( length, length, nullptr ), 0 );
}

inline Ref< DataBuffer > DataBuffer::create()
{
	return Ref< DataBuffer >( nullptr );
}

inline DataBuffer::DataBuffer(
	size_t length, size_t capacity, void const* data ) noexcept
	: m_length( length )
	, m_capacity( capacity )
{
	if( data )
	{
		memcpy( m_data, data, length );
	}
}

inline DataBuffer::~DataBuffer()
{
}

inline void DataBuffer::destroy()
{
	this->~DataBuffer();
	::operator delete( this );
}

inline Ref< DataBuffer > operator"" _db( char const* str, size_t len )
{
	return DataBuffer::create( len, str );
}
