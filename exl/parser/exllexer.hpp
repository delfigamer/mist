#pragma once

#include <exl/parser/charstream.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <memory>
#include <stack>
#include <vector>

namespace exl
{
	enum class ExlTokenType
	{
		identifier,
		number,
		string,
		eof,
		newline,

		kwas,
		kwclass,
		kwconst,
		kwelse,
		kwelseif,
		kwend,
		kwfunction,
		kwif,
		kwin,
		kwinout,
		kwlocal,
		kwnil,
		kwout,
		kwthen,
		kwtype,
		kwunit,

		leftroundbracket,
		rightroundbracket,
		leftsquarebracket,
		rightsquarebracket,
		leftbrace,
		rightbrace,
		semicolon,
		colon,
		bang,
		equals,
		equalsequals,
		plus,
		plusequals,
		minus,
		minusequals,
		star,
		starequals,
		slash,
		slashequals,
		tilde,
		tildeequals,
		pound,
		comma,
		dot,
		dotdot,
		dotdotdot,
	};

	namespace lexer
	{
		bool keyword( std::vector< uint8_t > word, ExlTokenType* kw );
	}

	struct ExlToken
	{
		ExlTokenType type;
		bool bislinestart;
		TextPos spos;
		TextPos epos;
		Ref< DataBuffer > data;
		double number;

		ExlToken( ExlTokenType type, TextPos spos, TextPos epos )
			: type( type )
			, bislinestart( false )
			, spos( spos )
			, epos( epos )
		{
		}
		ExlToken( ExlTokenType type, TextPos spos, TextPos epos, double number )
			: type( type )
			, bislinestart( false )
			, spos( spos )
			, epos( epos )
			, number( number )
		{
		}
		ExlToken( ExlTokenType type, TextPos spos, TextPos epos,
			Ref< DataBuffer > const& data )
			: type( type )
			, bislinestart( false )
			, spos( spos )
			, epos( epos )
			, data( data )
		{
		}
		ExlToken( ExlTokenType type, TextPos spos, TextPos epos,
			Ref< DataBuffer >&& data )
			: type( type )
			, bislinestart( false )
			, spos( spos )
			, epos( epos )
			, data( std::move( data ) )
		{
		}
	};

	class ExlTokenStream
	{
	private:
		CharStream cs;
		std::stack< std::unique_ptr< ExlToken > > buffer;

	public:
		Ref< DataBuffer > filename;

	public:
		ExlTokenStream(
			char const* const* source, size_t const* sourcelengths,
			size_t partcount, char const* filename, int tabsize );
		~ExlTokenStream();

		std::unique_ptr< ExlToken > gett();
		void ungett( std::unique_ptr< ExlToken >&& token );
		std::unique_ptr< ExlToken > const& peekt();
		ATTRIBUTE(( __noreturn__ ))
		void error( char const* message, TextPos spos, TextPos epos )
		{
			cs.error( message, spos, epos );
		}
		ATTRIBUTE(( __noreturn__ ))
		void error( char const* message, TextPos pos )
		{
			cs.error( message, pos );
		}
	};
}
