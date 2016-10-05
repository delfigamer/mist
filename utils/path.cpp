#include <utils/path.hpp>
#include <utils/pathparse.hpp>
#include <utils/configset.hpp>
#include <osapi.hpp>
#include <map>
#include <string>
#include <cstring>

namespace utils
{
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
		size_t size = m_target->m_length + sizeof( char_t );
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
				( ( char_t* )data )[ -1 ] = PATH_SEP[ 0 ];
				data -= current->m_result->m_length;
				memcpy(
					data,
					current->m_result->m_data,
					current->m_result->m_length - sizeof( char_t ) );
				break;
			}
			( ( char_t* )data )[ -1 ] = PATH_SEP[ 0 ];
			data -= sizeof( char_t ) + current->m_target->m_length;
			memcpy( data, current->m_target->m_data, current->m_target->m_length );
			current = current->m_base;
		}
		return ( char_t const* )m_result->m_data;
	}

	namespace
	{
		std::map< std::string, Ref< Path > > rootmap;
		Ref< Path > defaultbase;

		Ref< Path > rootpath( char const* path )
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			Ref< DataBuffer > wpath = intern( ( uint8_t const* )path, 0 );
			int cwdlength = GetFullPathNameW( LPCWSTR( wpath->m_data ), 0, 0, 0 );
			Ref< DataBuffer > db = DataBuffer::create( 0, cwdlength * 2 + 10, 0 );
			uint8_t* data = db->m_data;
			memcpy( data, L"\\\\\?\\", 8 );
			cwdlength = GetFullPathNameW(
				LPCWSTR( wpath->m_data ), cwdlength + 1, LPWSTR( data + 8 ), 0 );
			wchar_t* buf = ( wchar_t* )data;
			if( buf[ cwdlength + 3 ] == '\\' )
			{
				db->m_length = cwdlength * 2 + 6;
			}
			else
			{
				db->m_length = cwdlength * 2 + 8;
			}
			return Ref< Path >::create( db, nullptr );
#else
			size_t length = strlen( path );
			while(
				path[ length - 1 ] <= 32 ||
				path[ length - 1 ] == '/' ||
				path[ length - 1 ] == '\\' )
			{
				length -= 1;
			}
			Ref< DataBuffer > db = DataBuffer::create(
				length, length, path );
			return Ref< Path >::create( db, nullptr );
#endif
		}
	}

	void Path::initialize()
	{
		defaultbase = rootpath( PATH_START );
		size_t count = MainConf->integer( "#path", 0 );
		for( size_t i = 1; i <= count; ++i )
		{
			String nameexpr = String::format( "path[%llu][1]", i );
			String targetexpr = String::format( "path[%llu][2]", i );
			String name = MainConf->string( nameexpr, nullptr );
			String target = MainConf->string( targetexpr, nullptr );
			if( name && target )
			{
				Ref< Path > path = rootpath( target );
				rootmap.emplace( name.getchars(), path );
			}
		}
	}

	namespace
	{
		template< typename char_t >
		void pointmove(
			Ref< Path >* pcurrent, char_t const* point, size_t length )
		{
			if( !point )
			{
			}
			else if( point[ 0 ] == '.' && length == 1 )
			{
			}
			else if( point[ 0 ] == '.' && point[ 1 ] == '.' && length == 2 )
			{
				*pcurrent = ( *pcurrent )->m_base;
				if( !*pcurrent )
				{
					throw std::runtime_error( "invalid path" );
				}
			}
			else
			{
				*pcurrent = Ref< Path >::create(
					intern( point, length ), *pcurrent );
			}
		}
	}

	Ref< Path > Path::create( void const* path )
	{
		Ref< Path > current;
		uint8_t const* pathb = ( uint8_t const* )path;
#if defined( _WIN32 ) || defined( _WIN64 )
		if( pathb[ 0 ] == 0xff && pathb[ 1 ] == 0xfe )
		{
			uint16_t const* pathh = ( uint16_t const* )( pathb + 2 );
			uint16_t const* point;
			size_t length = 0;
			while( nextpoint( &pathh, &point, &length ) )
			{
				pointmove( &current, point, length );
			}
			return current;
		}
#endif
		uint8_t const* point;
		size_t length;
		nextpoint( &pathb, &point, &length );
		if( point )
		{
			std::string pointstd(
				( char const* )point, ( char const* )point + length );
			auto it = rootmap.find( pointstd );
			if( it != rootmap.end() )
			{
				current = it->second;
			}
			else
			{
				current = defaultbase;
				pointmove( &current, point, length );
			}
		}
		else
		{
			current = defaultbase;
		}
		while( nextpoint( &pathb, &point, &length ) )
		{
			pointmove( &current, point, length );
		}
		return current;
	}
}
