#include <exl/func.hpp>
#include <exl/node/block.hpp>
#include <exl/node/expr/invoke.hpp>
#include <exl/node/expr/name.hpp>
#include <exl/node/expr/number.hpp>
#include <exl/node/expr/string.hpp>
#include <exl/node/stat/const.hpp>
#include <exl/node/stat/expression.hpp>
#include <exl/node/stat/local.hpp>
#include <exl/parser/ast.hpp>
#include <exl/parser/exllexer.hpp>
#include <vector>
#include <cstring>

namespace exl
{
	using utils::SExpr;

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

		ATTRIBUTE(( noreturn ))
		void construct_error( char const* message, utils::SExpr const& s )
		{
			if( s.head < 0x40000000 &&
				s.list.size() >= 1 &&
				s[ 1 ].head == NodeHead::textrange &&
				s[ 1 ][ 1 ].head == SExpr::head_number &&
				s[ 1 ][ 2 ].head == SExpr::head_number &&
				s[ 1 ][ 3 ].head == SExpr::head_number &&
				s[ 1 ][ 4 ].head == SExpr::head_number &&
				s[ 1 ][ 5 ].head == SExpr::head_text )
			{
				char const* filename;
				int filenamelen;
				if( s[ 1 ][ 5 ].text )
				{
					filenamelen = s[ 1 ][ 5 ].text->m_length;
					filename = ( char const* )s[ 1 ][ 5 ].text->m_data;
				}
				else
				{
					filenamelen = 0;
					filename = "";
				}
				throw ExlError( message,
					s[ 1 ][ 1 ].number, s[ 1 ][ 2 ].number,
					s[ 1 ][ 3 ].number, s[ 1 ][ 4 ].number,
					filename, filenamelen );
			}
			else
			{
				throw ExlError( message );
			}
		}
	}

	Ref< DataBuffer > const& lineprefix( size_t depth )
	{
		static std::vector< Ref< DataBuffer > > lps{ nullptr };
		while( depth >= lps.size() )
		{
			size_t i = lps.size();
			auto db = DataBuffer::create( i, i, nullptr );
			memset( db->m_data, '\t', i );
			lps.push_back( std::move( db ) );
		}
		return lps[ depth ];
	}

	Ref< DataBuffer > numtostr( double number )
	{
		double sn;
		int precision = 0;
		char str[ 256 ];
		int len;
		do
		{
			len = snprintf( str, sizeof( str ), "%.*lg", precision, number );
			sscanf( str, "%lg", &sn );
			precision += 1;
		}
			while( sn != number );
		return DataBuffer::create( len, len, str );
	}

	Ref< DataBuffer > bytestostr( Ref< DataBuffer > const& bytes )
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
		return DataBuffer::create( ret.size(), ret.size(), ret.data() );
	}

	Ref< DataBuffer > identtostr( identifier_t const& ident )
	{
		if( ident.empty() )
		{
			return nullptr;
		}
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
			Ref< DataBuffer > ret = DataBuffer::create(
				ident.size() + 1, ident.size() + 1, nullptr );
			ret->m_data[ 0 ] = '@';
			memcpy( ret->m_data + 1, ident.data(), ident.size() );
			return ret;
		}
		else
		{
			return DataBuffer::create(
				ident.size(), ident.size(), ident.data() );
		}
	}

	identifier_t constructidentifier( utils::SExpr const& s )
	{
		if( s.head == SExpr::head_text )
		{
			if( s.text )
			{
				return identifier_t(
					s.text->m_data, s.text->m_data + s.text->m_length );
			}
			else
			{
				return identifier_t();
			}
		}
		else
		{
			construct_error( "invalid identifier node", s );
		}
	}

	Ref< IBlock > constructblock( utils::SExpr const& s )
	{
		switch( s.head )
		{
		case NodeHead::block:
			return Ref< Block >::create( s );
		default:
			construct_error( "invalid block node", s );
		}
	}

	Ref< IStatement > constructstatement( utils::SExpr const& s )
	{
		switch( s.head )
		{
		case NodeHead::stat_const:
			return Ref< ConstStat >::create( s );
		case NodeHead::stat_expression:
			return Ref< ExpressionStat >::create( s );
		case NodeHead::stat_local:
			return Ref< LocalStat >::create( s );
		default:
			construct_error( "invalid statement node", s );
		}
	}

	Ref< IExpression > constructexpression( utils::SExpr const& s )
	{
		switch( s.head )
		{
		case NodeHead::expr_invoke:
			return Ref< InvokeExpr >::create( s );
		case NodeHead::expr_name:
			return Ref< NameExpr >::create( s );
		case NodeHead::expr_number:
			return Ref< NumberExpr >::create( s );
		case NodeHead::expr_string:
			return Ref< StringExpr >::create( s );
		default:
			construct_error( "invalid expression node", s );
		}
	}
}
