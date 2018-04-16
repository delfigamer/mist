#pragma once

#include <common/databuffer.hpp>
#include <common/ref.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <utils/encoding.hpp>
#endif
#include <common.hpp>
#include <osapi.hpp>
#include <stdexcept>
#include <cstring>

namespace utils
{
	namespace
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		Ref< DataBuffer > intern(
			uint8_t const* begin, uint8_t const* end = nullptr )
		{
			if( begin == end )
			{
				return DataBuffer::create( 0 );
			}
			translation_t translation =
			{
				&encoding::utf8,
				&encoding::utf16,
				begin,
				0,
				end ? size_t( end - begin ) : 0,
				0,
				0,
			};
			if( translatestr( translation )
				!= translateresult::success )
			{
				throw std::runtime_error( "invalid UTF-8 string" );
			}
			Ref< DataBuffer > db = DataBuffer::create( translation.destresult );
			translation.dest = db->m_data;
			translation.destsize = translation.destresult;
			if( translatestr( translation )
				!= translateresult::success )
			{
				throw std::runtime_error( "invalid UTF-8 string" );
			}
			return db;
		}

		Ref< DataBuffer > intern( uint16_t const* begin, uint16_t const* end )
		{
			return DataBuffer::create( ( end - begin ) * 2, begin );
		}
#else
		Ref< DataBuffer > intern( uint8_t const* begin, uint8_t const* end )
		{
			return DataBuffer::create( end - begin, begin );
		}

		Ref< DataBuffer > intern( uint8_t const* begin )
		{
			return DataBuffer::create( end - begin, begin );
		}
#endif
		template< typename char_t >
		char_t const* findseparator( char_t const* str )
		{
			while( true )
			{
				switch( str[ 0 ] )
				{
				case '\0':
				case '\\':
				case '/':
					return str;

				default:
					++str;
				}
			}
		}

		template< typename char_t >
		char_t const* skipseparator( char_t const* str )
		{
			while( str[ 0 ] != '\0' &&
				( str[ 0 ] <= ' ' || str[ 0 ] == '\\' || str[ 0 ] == '/' ) )
			{
				++str;
			}
			return str;
		}

		template< typename char_t >
		void trim( char_t const*& begin, char_t const*& end )
		{
			while( begin < end && begin[ 0 ] <= ' ' )
			{
				++begin;
			}
			while( begin < end && end[ -1 ] <= ' ' )
			{
				--end;
			}
		}

		template< typename char_t >
		void nextpoint(
			char_t const*& str, char_t const*& begin, char_t const*& end )
		{
			begin = str;
			end = findseparator( str );
			str = skipseparator( end );
			trim( begin, end );
		}
	}
}
