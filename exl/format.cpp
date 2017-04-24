#include <exl/format.hpp>
#include <exl/parser/exllexer.hpp>
#include <common/stringbuilder.hpp>
#include <cstring>
#include <cmath>

namespace exl
{
	namespace
	{
		void bytestostr_char(
			std::vector< uint8_t >& ret, uint8_t c, uint8_t next )
		{
			if( c == '\n' || c == '"' || c == '\\' )
			{
				ret.push_back( '\\' );
				ret.push_back( c );
			}
			else if( c < 32 )
			{
				ret.push_back( '\\' );
				if( next >= '0' && next <= '9' )
				{
					ret.push_back( '0' );
					ret.push_back( '0' + c / 10 );
					ret.push_back( '0' + c % 10 );
				}
				else if( c >= 10 )
				{
					ret.push_back( '0' + c / 10 );
					ret.push_back( '0' + c % 10 );
				}
				else
				{
					ret.push_back( '0' + c );
				}
			}
			else
			{
				ret.push_back( c );
			}
		}
	}

	Ref< DataBuffer > lineprefix( size_t depth )
	{
		static std::vector< Ref< DataBuffer > > lps{ nullptr };
		while( depth >= lps.size() )
		{
			size_t i = lps.size();
			auto db = DataBuffer::create( i );
			memset( db->m_data, '\t', i );
			lps.push_back( std::move( db ) );
		}
		return lps[ depth ];
	}

	Ref< DataBuffer > inttostr( int number )
	{
		char str[ 256 ];
		int len = snprintf( str, sizeof( str ), "%i", number );
		return DataBuffer::create( len, str );
	}

	Ref< DataBuffer > textpostostr( TextPos pos )
	{
		char str[ 256 ];
		int len = snprintf( str, sizeof( str ), "%i:%i", pos.row, pos.col );
		return DataBuffer::create( len, str );
	}

	Ref< DataBuffer > textrangetostr( TextRange range )
	{
		if( range.filename )
		{
			char str[ 2048 ];
			int len = snprintf( str, sizeof( str ), "%.*s:%i:%i~%i:%i",
				( int )range.filename->m_length,
				( char const* )range.filename->m_data,
				range.spos.row, range.spos.col,
				range.epos.row, range.epos.col );
			return DataBuffer::create( len, str );
		}
		else
		{
			char str[ 256 ];
			int len = snprintf( str, sizeof( str ), ":%i:%i~%i:%i",
				range.spos.row, range.spos.col,
				range.epos.row, range.epos.col );
			return DataBuffer::create( len, str );
		}
	}

	Ref< DataBuffer > defpostostr( DefPos defpos )
	{
		if( defpos.filename )
		{
			char str[ 2048 ];
			int len = snprintf( str, sizeof( str ), "%.*s:%i:%i",
				( int )defpos.filename->m_length,
				( char const* )defpos.filename->m_data,
				defpos.pos.row, defpos.pos.col );
			return DataBuffer::create( len, str );
		}
		else
		{
			char str[ 256 ];
			int len = snprintf( str, sizeof( str ), ":%i:%i",
				defpos.pos.row, defpos.pos.col );
			return DataBuffer::create( len, str );
		}
	}

	Ref< DataBuffer > numtostr( double number )
	{
		double sn;
		int precision = 0;
		char str[ 256 ];
		int len;
		char const* format;
		if( ( number != 0 && fabs( number ) < 1e-6 ) || fabs( number ) >= 1e6 )
		{
			format = "%.*le";
		}
		else
		{
			format = "%.*lf";
		}
		do
		{
			len = snprintf( str, sizeof( str ), format, precision, number );
			sscanf( str, "%lf", &sn );
			precision += 1;
		}
			while( sn != number );
		return DataBuffer::create( len, str );
	}

	Ref< DataBuffer > bytestostr( DataBuffer* bytes )
	{
		if( !bytes )
		{
			return nullptr;
		}
		std::vector< uint8_t > ret;
		uint8_t* data = bytes->m_data;
		size_t length = bytes->m_length;
		for( size_t i = 1; i < length; ++i )
		{
			bytestostr_char( ret, data[ i-1 ], data[ i ] );
		}
		if( length != 0 )
		{
			bytestostr_char( ret, data[ length-1 ], 0 );
		}
		return DataBuffer::create( ret.size(), ret.data() );
	}

	Ref< DataBuffer > identtostr( identifier_t const& ident )
	{
		bool needsescape =
			ident.size() == 0 ||
			( ident[ 0 ] >= '0' && ident[ 0 ] <= '9' );
		if( !needsescape )
		{
			ExlTokenType tokentype;
			needsescape = lexer::keyword( ident, &tokentype );
		}
		if( needsescape )
		{
			Ref< DataBuffer > ret = DataBuffer::create( ident.size() + 1 );
			ret->m_data[ 0 ] = '@';
			memcpy( ret->m_data + 1, ident.data(), ident.size() );
			return ret;
		}
		else
		{
			return DataBuffer::create( ident.size(), ident.data() );
		}
	}

	Ref< DataBuffer > nametostr( name_t const& name, uint8_t delimiter )
	{
		if( name.empty() )
		{
			return nullptr;
		}
		std::vector< Ref< DataBuffer > > parts;
		parts.reserve( name.size() );
		size_t length = 0;
		for( auto& ident : name )
		{
			Ref< DataBuffer > part = identtostr( ident );
			length += part->m_length + 1;
			parts.push_back( part );
		}
		Ref< DataBuffer > ret = DataBuffer::create( length - 1 );
		uint8_t* buffer = ret->m_data;
		auto it = parts.begin();
		auto eit = parts.end();
		--eit;
		while( it != eit )
		{
			memcpy( buffer, ( *it )->m_data, ( *it )->m_length );
			buffer += ( *it )->m_length;
			buffer[ 0 ] = delimiter;
			buffer += 1;
			++it;
		}
		memcpy( buffer, ( *it )->m_data, ( *it )->m_length );
		return ret;
	}
}
