#pragma once

#include <exl/exlerror.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	struct TextPos
	{
		int row;
		int col;
	};

	struct CharStream
	{
		uint8_t const* const* source;
		size_t const* sourcelengths;
		size_t partcount;
		char const* filename;
		int tabsize;
		size_t currentpart = 0;
		size_t position = 0;
		TextPos textpos;
		int prevlinelength = -1;

		void movenext();
		void moveprev();
		int getc();
		void ungetc();
		ATTRIBUTE(( __noreturn__ ))
		void error( char const* message, TextPos spos, TextPos epos )
		{
			throw ExlError(
				message, spos.row, spos.col, epos.row, epos.col, filename );
		}
		ATTRIBUTE(( __noreturn__ ))
		void error( char const* message, TextPos pos )
		{
			throw ExlError( message, pos.row, pos.col, filename );
		}
	};
}
