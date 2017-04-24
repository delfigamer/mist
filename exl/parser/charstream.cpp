#include <exl/parser/charstream.hpp>
#include <exl/func.hpp>
#include <common/strexception.hpp>

namespace exl
{
	void CharStream::movenext()
	{
		position += 1;
		while( currentpart < partcount &&
			position >= sourcelengths[ currentpart ] )
		{
			position = 0;
			currentpart += 1;
		}
	}

	void CharStream::moveprev()
	{
		if( position == 0 )
		{
			do
			{
				currentpart -= 1;
			}
				while( sourcelengths[ currentpart ] == 0 );
			position = sourcelengths[ currentpart ] - 1;
		}
		else
		{
			position -= 1;
		}
	}

	int CharStream::getc()
	{
		if( currentpart >= partcount )
		{
			position += 1;
			return -1;
		}
		int ch = source[ currentpart ][ position ];
		movenext();
		if( ch == '\n' or ch == '\r' )
		{
			if( currentpart < partcount )
			{
				int nch = source[ currentpart ][ position ];
				if( ( nch == '\n' || nch == '\r' ) && nch != ch )
				{
					movenext();
				}
			}
			prevlinelength = textpos.col;
			textpos.row += 1;
			textpos.col = 1;
			return '\n';
		}
		else
		{
			if( ch == '\t' )
			{
				textpos.col += tabsize;
			}
			else
			{
				textpos.col += 1;
			}
			return ch;
		}
	}

	void CharStream::ungetc()
	{
		moveprev();
		if( currentpart >= partcount )
		{
			return;
		}
		int ch = source[ currentpart ][ position ];
		if( ch == '\n' || ch == '\r' )
		{
			if( currentpart != 0 && position != 0 )
			{
				moveprev();
				int pch = source[ currentpart ][ position ];
				if( ( pch != '\n' && pch != '\r' ) || pch == ch )
				{
					movenext();
				}
			}
			textpos.row -= 1;
			textpos.col = prevlinelength;
			prevlinelength = -1;
		}
		else
		{
			if( ch == '\t' )
			{
				textpos.col -= tabsize;
			}
			else
			{
				textpos.col -= 1;
			}
		}
	}

	void CharStream::error( char const* message, TextPos spos, TextPos epos )
	{
		throw StrException( "%s:%i:%i~%i:%i %s",
			filename ? filename : "",
			spos.row, spos.col, epos.row, epos.col,
			message );
	}

	void CharStream::error( char const* message, TextPos pos )
	{
		throw StrException( "%s:%i:%i %s",
			filename ? filename : "",
			pos.row, pos.col,
			message );
	}
}
