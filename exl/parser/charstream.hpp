#pragma once

#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
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
		void error( char const* message, TextPos spos, TextPos epos );
		ATTRIBUTE(( __noreturn__ ))
		void error( char const* message, TextPos pos );
	};
}
