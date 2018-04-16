#include <utils/path.hpp>
#include <utils/pathparse.hpp>
#include <utils/configset.hpp>
#include <common/stringbuilder.hpp>
#include <osapi.hpp>
#include <map>
#include <vector>
#include <string>
#include <cstring>

namespace utils
{
	Path::Path( DataBuffer* point, Path* base )
		: m_point( point )
		, m_base( base )
	{
	}

	Path::Path( Path const& other )
		: m_point( other.m_point )
		, m_base( other.m_base )
	{
	}

	Path::Path( Path&& other )
		: m_point( std::move( other.m_point ) )
		, m_base( std::move( other.m_base ) )
	{
	}

	Path::~Path()
	{
	}

	Path::char_t const* Path::combine()
	{
		if( !m_fullpath )
		{
			StringBuilder fullpath =
				m_point + ( sizeof( char_t ) == 1 ? "\0"_db : "\0\0"_db );
			Path* current = m_base;
			while( current )
			{
				fullpath = current->m_point + PATH_SEP_DB + fullpath;
				current = current->m_base;
			}
			m_fullpath = fullpath.combine();
		}
		return ( char_t const* )m_fullpath->m_data;
	}

	namespace
	{
		struct DataBufferComparer
		{
			bool operator()(
				DataBuffer* left,
				DataBuffer* right ) const
			{
				if( !left )
				{
					return ( bool )right;
				}
				if( !right )
				{
					return false;
				}
				size_t commonlen = left->m_length;
				if( commonlen > right->m_length )
				{
					commonlen = right->m_length;
				}
				int mc = memcmp( left->m_data, right->m_data, commonlen );
				if( mc != 0 )
				{
					return mc < 0;
				}
				return left->m_length < right->m_length;
			}
		};

		std::map< Ref< DataBuffer >, Ref< Path >, DataBufferComparer > rootmap;

		Ref< Path > rootpath( char const* path )
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			Ref< DataBuffer > wpath = intern( ( uint8_t const* )path );
			if( wpath->m_length >= 8 &&
				memcmp( wpath->m_data, L"\\\\?\\", 8 ) == 0 )
			{
				return Ref< Path >::create( wpath, nullptr );
			}
			int cwdlength = GetFullPathNameW( LPCWSTR( wpath->m_data ), 0, 0, 0 );
			Ref< DataBuffer > db = DataBuffer::create(
				0, cwdlength * 2 + 10, nullptr );
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
			Ref< DataBuffer > db = DataBuffer::create( length, path );
			return Ref< Path >::create( db, nullptr );
#endif
		}
	}

	void Path::initialize()
	{
		int i = 1;
		while( true )
		{
			char nameexpr[ 256 ];
			char targetexpr[ 256 ];
			snprintf( nameexpr, sizeof( nameexpr ), "path[%i][1]", i );
			snprintf( targetexpr, sizeof( targetexpr ), "path[%i][2]", i );
			std::string name = MainConf->string( nameexpr );
			std::string target = MainConf->string( targetexpr );
			if( target.empty() )
			{
				break;
			}
			Ref< DataBuffer > iname = intern(
				( uint8_t const* )&*name.begin(),
				( uint8_t const* )&*name.end() );
			Ref< Path > path = rootpath( target.c_str() );
#if defined( _WIN32 ) || defined( _WIN64 )
			LOG( "root mapping: '%.*ls' -> '%.*ls'",
				( int )iname->m_length/2,
				( wchar_t const* )iname->m_data,
				( int )path->m_point->m_length/2,
				( wchar_t const* )path->m_point->m_data );
#else
			LOG( "root mapping: '%.*s' -> '%.*s'",
				( int )iname->m_length,
				( char const* )iname->m_data,
				( int )path->m_point->m_length,
				( char const* )path->m_point->m_data );
#endif
			rootmap.emplace( iname, path );
			++i;
		}
	}

	namespace
	{
		void pointmove( Ref< Path >& current, DataBuffer* point )
		{
			if( !point )
			{
				return;
			}
			Path::char_t const* begin =
				( Path::char_t const* )point->m_data;
			Path::char_t const* end =
				( Path::char_t const* )( point->m_data + point->m_length );
			if( begin == end )
			{
			}
			else if(
				begin + 1 == end &&
				begin[ 0 ] == '.' )
			{
			}
			else if(
				begin + 2 == end &&
				begin[ 0 ] == '.' &&
				begin[ 1 ] == '.' )
			{
				current = current->m_base;
			}
			else
			{
				current = Ref< Path >::create( point, current );
			}
		}

		template< typename char_t >
		Ref< Path > createpath( char_t const* path )
		{
			Ref< Path > current;
			char_t const* pointbegin;
			char_t const* pointend;
			nextpoint( path, pointbegin, pointend );
			Ref< DataBuffer > ipoint = intern( pointbegin, pointend );
			auto it = rootmap.find( ipoint );
			if( it != rootmap.end() )
			{
				current = it->second;
			}
			else
			{
				return nullptr;
			}
			while( true )
			{
				nextpoint( path, pointbegin, pointend );
				if( pointbegin == pointend )
				{
					break;
				}
				pointmove( current, intern( pointbegin, pointend ) );
				if( !current )
				{
					return nullptr;
				}
			}
			return current;
		}
	}

	Ref< Path > Path::create( void const* path )
	{
		assert( path );
		uint8_t const* pathb = ( uint8_t const* )path;
#if defined( _WIN32 ) || defined( _WIN64 )
		if( pathb[ 0 ] == 0xff && pathb[ 1 ] == 0xfe )
		{
			return createpath( ( uint16_t const* )( pathb + 2 ) );
		}
#endif
		return createpath( pathb );
	}
}
