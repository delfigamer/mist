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
		Ref< DataBuffer > intern( uint8_t const* str, size_t length )
		{
			translation_t translation =
			{
				&encoding::utf8,
				&encoding::utf16,
				str,
				0,
				length,
				0,
				0,
			};
			if( translatestr( &translation ) != translateresult::success )
			{
				throw std::runtime_error( "invalid UTF-8 string" );
			}
			Ref< DataBuffer > db = DataBuffer::create( translation.destresult );
			translation.dest = db->m_data;
			translation.destsize = translation.destresult;
			if( translatestr( &translation ) != translateresult::success )
			{
				throw std::runtime_error( "invalid UTF-8 string" );
			}
			return db;
		}

		Ref< DataBuffer > intern( uint16_t const* str, size_t length )
		{
			return DataBuffer::create( length * 2, str );
		}
#else
		Ref< DataBuffer > intern( uint8_t const* str, int length )
		{
			return DataBuffer::create( length, str );
		}
#endif
		template< typename char_t >
		size_t findsep( char_t const* str )
		{
			for( char_t const* ch = str; true; ++ch )
			{
				if( *ch == 0 || *ch == '\\' || *ch == '/' )
				{
					return ch - str;
				}
			}
		}

		template< typename char_t >
		char_t const* skipsep( char_t const* str )
		{
			while( *str != 0 && ( *str <= 32 || *str == '\\' || *str == '/' ) )
			{
				++str;
			}
			return str;
		}

		template< typename char_t >
		bool trim( char_t const** pstr, size_t* plength )
		{
			char_t const* str = *pstr;
			size_t length = *plength;
			for( size_t i = 0; i < length; ++i )
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
			for( size_t i = length - 1; i >= 0; --i )
			{
				if( str[ i ] > 32 )
				{
					*pstr = str;
					*plength = i + 1;
					return true;
				}
			}
			ASSERT( false );
			return false;
		}

		template< typename char_t >
		bool nextpoint(
			char_t const** pstr, char_t const** ppoint, size_t* plength )
		{
			char_t const* str = *pstr;
			if( str[ 0 ] == 0 )
			{
				return false;
			}
			size_t length = findsep( str );
			*pstr = skipsep( str + length );
			if( trim( &str, &length ) )
			{
				*ppoint = str;
				*plength = length;
			}
			else
			{
				*ppoint = 0;
			}
			return true;
		}
	}
}
