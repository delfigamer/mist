#include "path.hpp"
#if defined( _WIN32 ) || defined( _WIN64 )
#include "encoding.hpp"
#endif
#include <cstring>
#if defined( _WIN32 ) || defined( _WIN64 )
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

namespace utils
{
#if defined( _WIN32 ) || defined( _WIN64 )
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif
	Path::Path( DataBuffer* target, Path* base )
		: m_target( target )
		, m_base( base )
	{
	}

	Path::Path( Path const& other )
		: m_target( other.m_target )
		, m_base( other.m_base )
	{
	}

	Path::Path( Path&& other )
		: m_target( std::move( other.m_target ) )
		, m_base( std::move( other.m_base ) )
	{
	}

	Path::~Path()
	{
	}

	Path::char_t const* Path::combine()
	{
		if( m_result )
		{
			return ( char_t const* )m_result->m_data;
		}
		int size = m_target->m_length + sizeof( char_t );
		Path* current = m_base;
		while( current )
		{
			if( current->m_result )
			{
				size += current->m_result->m_length;
				break;
			}
			size += sizeof( char_t ) + current->m_target->m_length;
			current = current->m_base;
		}
		m_result = DataBuffer::create( size, size, 0 );
		uint8_t* data = m_result->m_data + size;
		( ( char_t* )data )[ -1 ] = 0;
		data -= sizeof( char_t ) + m_target->m_length;
		memcpy( data, m_target->m_data, m_target->m_length );
		current = m_base;
		while( current )
		{
			if( current->m_result )
			{
				( ( char_t* )data )[ -1 ] = PATH_SEP;
				data -= current->m_result->m_length;
				memcpy(
					data,
					current->m_result->m_data,
					current->m_result->m_length - sizeof( char_t ) );
				break;
			}
			( ( char_t* )data )[ -1 ] = PATH_SEP;
			data -= sizeof( char_t ) + current->m_target->m_length;
			memcpy( data, current->m_target->m_data, current->m_target->m_length );
			current = current->m_base;
		}
		return ( char_t const* )m_result->m_data;
	}

	static Path* getdefaultbase()
	{
		static Ref< Path > defaultbase;
		if( !defaultbase )
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			int cwdlength = GetCurrentDirectoryW( 0, 0 ) - 1;
			int buflen = cwdlength * 2 + 8;
			Ref< DataBuffer > db = DataBuffer::create( buflen, buflen, 0 );
			memcpy( db->m_data, L"\\\\\?\\", 8 );
			GetCurrentDirectoryW( cwdlength + 1, ( LPWSTR )( db->m_data + 8 ) );
			defaultbase = Ref< Path >::create( db, nullptr );
			defaultbase->combine();
#else
#endif
		}
		return defaultbase;
	}
#if defined( _WIN32 ) || defined( _WIN64 )
	static Ref< DataBuffer > intern( uint8_t const* str, int length )
	{
		translation_t translation =
		{
			&encoding::utf8,
			&encoding::utf16,
			str,
			0,
			size_t( length ),
			0,
			0,
		};
		if( translatestr( &translation ) != translate_success )
		{
			throw std::runtime_error( "invalid UTF-8 string" );
		}
		Ref< DataBuffer > db = DataBuffer::create(
			translation.destresult, translation.destresult, 0 );
		translation.dest = db->m_data;
		translation.destsize = translation.destresult;
		if( translatestr( &translation ) != translate_success )
		{
			throw std::runtime_error( "invalid UTF-8 string" );
		}
		return db;
	}

	static Ref< DataBuffer > intern( uint16_t const* str, int length )
	{
		return DataBuffer::create( length * 2, length * 2, str );
	}
#else
	static Ref< DataBuffer > intern( uint8_t const* str, int length )
	{
		return DataBuffer::create( length, length, str );
	}
#endif
	template< typename char_t >
	static int findsep( char_t const* str )
	{
		for( int i = 0; true; ++i )
		{
			if( str[ i ] == 0 || str[ i ] == '\\' || str[ i ] == '/' )
			{
				return i;
			}
		}
	}

	template< typename char_t >
	static char_t const* skipsep( char_t const* str )
	{
		while( *str != 0 && ( *str <= 32 || *str == '\\' || *str == '/' ) )
		{
			++str;
		}
		return str;
	}

	template< typename char_t >
	static bool trim( char_t const** pstr, int* plength )
	{
		char_t const* str = *pstr;
		int length = *plength;
		for( int i = 0; i < length; ++i )
		{
			if( str[ i ] > 32 )
			{
				str = str + i;
				length = length - i;
				goto left_pass;
			}
		}
		return false;
	left_pass:
		for( int i = length - 1; i >= 0; --i )
		{
			if( str[ i ] > 32 )
			{
				*pstr = str;
				*plength = i + 1;
				return true;
			}
		}
		return false;
	}

	Ref< Path > Path::create( void const* path, Path* base )
	{
		Ref< Path > current = base ? base : getdefaultbase();
		uint8_t const* pathb = ( uint8_t const* )path;
#if defined( _WIN32 ) || defined( _WIN64 )
		if( pathb[ 0 ] == 0xff && pathb[ 1 ] == 0xfe )
		{
			uint16_t const* pathh = skipsep( ( uint16_t const* )( pathb + 2 ) );
			while( pathh[ 0 ] != 0 )
			{
				uint16_t const* partstr = pathh;
				int partlen = findsep( pathh );
				pathh = skipsep( pathh + partlen );
				if( trim( &partstr, &partlen ) )
				{
					if( partstr[ 0 ] == '.' && partlen == 1 )
					{
					}
					else if(
						partstr[ 0 ] == '.' && partstr[ 1 ] == '.' && partlen == 2 )
					{
						current = current->m_base;
						if( !current )
						{
							throw std::runtime_error( "invalid path" );
						}
					}
					else
					{
						current = Ref< Path >::create(
							intern( partstr, partlen ), current );
					}
				}
			}
			return current;
		}
#endif
		pathb = skipsep( pathb );
		while( pathb[ 0 ] != 0 )
		{
			uint8_t const* partstr = pathb;
			int partlen = findsep( pathb );
			pathb = skipsep( pathb + partlen );
			if( trim( &partstr, &partlen ) )
			{
				if( partstr[ 0 ] == '.' && partlen == 1 )
				{
				}
				else if(
					partstr[ 0 ] == '.' && partstr[ 1 ] == '.' && partlen == 2 )
				{
					current = current->m_base;
					if( !current )
					{
						throw std::runtime_error( "invalid path" );
					}
				}
				else
				{
					current = Ref< Path >::create(
						intern( partstr, partlen ), current );
				}
			}
		}
		return current;
	}
}
