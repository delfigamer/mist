#include <utils/strexception.hpp>
#include <utils/configset.hpp>
#include <utils/string.hpp>
#include <utils/encoding.hpp>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstdio>
#include <cinttypes>

void CriticalWinError(
	char const* file, char const* function, int line, char const* msg )
{
	char StrBuffer[ 1024 ];
	snprintf( StrBuffer, sizeof( StrBuffer ),
		"[%s : %s @ %i] %s", file, function, line, msg );
	throw utils::StrException( StrBuffer );
}

static TEXTMETRIC TextMetric;

void CheckWinError( char const* file, char const* function, int line )
{
	DWORD LastError = GetLastError();
	if( !LastError )
	{
		return;
	}
	char StrBuffer[ 1024 ];
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		0,
		LastError,
		MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
		StrBuffer,
		sizeof( StrBuffer ) - 1,
		0 );
	CriticalWinError( file, function, line, StrBuffer );
}
#define CheckWinError() CheckWinError( __FILE__, __FUNCTION__, __LINE__ )

static wchar_t const* trimpath( wchar_t const* argstr )
{
	if( argstr[ 0 ] == '"' )
	{
		int rpos = 1;
		while( argstr[ rpos ] )
		{
			if( argstr[ rpos ] == '\\' && argstr[ rpos + 1 ] )
			{
				rpos += 2;
			}
			else if( argstr[ rpos ] == '"' )
			{
				rpos += 1;
				break;
			}
			else
			{
				rpos += 1;
			}
		}
		return argstr + rpos;
	}
	else
	{
		int rpos = 1;
		while( argstr[ rpos ] )
		{
			if( uint16_t( argstr[ rpos ] ) <= uint16_t( ' ' ) )
			{
				rpos += 1;
				break;
			}
			else
			{
				rpos += 1;
			}
		}
		return argstr + rpos;
	}
}

utils::String convertstr(
		void const* source,
		utils::encoding_t const& senc,
		utils::encoding_t const& denc )
{
	utils::translation_t translation =
	{
		&senc,
		&denc,
		source,
		0,
		0,
		0,
		0xfffd,
	};
	if( utils::translatestr( &translation ) != utils::translate_success )
	{
		throw std::runtime_error( "encoding failure" );
	}
	utils::Ref< utils::DataBuffer > db = utils::DataBuffer::create(
		translation.destresult, translation.destresult, 0 );
	translation.dest = db->m_data;
	translation.destsize = db->m_capacity;
	if( utils::translatestr( &translation ) != utils::translate_success )
	{
		throw std::runtime_error( "encoding failure" );
	}
	return utils::String( db );
}

void writebeu16( uint16_t i, FILE* f )
{
	uint8_t buf[ 2 ] =
	{
		uint8_t( i >> 8 ),
		uint8_t( i ),
	};
	fwrite( buf, 1, 2, f );
}

void writebes16( int16_t i, FILE* f )
{
	return writebeu16( uint16_t( i ), f );
}

void writebeu32( uint32_t i, FILE* f )
{
	uint8_t buf[ 4 ] =
	{
		uint8_t( i >> 24 ),
		uint8_t( i >> 16 ),
		uint8_t( i >> 8 ),
		uint8_t( i ),
	};
	fwrite( buf, 1, 4, f );
}

struct bitmap_t
{
	int m_stride;
	int m_height;
	uint8_t m_data[];

	static bitmap_t* create( int width, int height )
	{
		int stride = ( width + 7 ) >> 3;
		bitmap_t* bm = ( bitmap_t* )operator new(
			sizeof( bitmap_t ) + stride * height );
		bm->m_stride = stride;
		bm->m_height = height;
	}

	void release()
	{
		operator delete( this );
	}

	void clear()
	{
		memset( m_data, 0, m_stride * m_height );
	}

	void set( int x, int y, bool value )
	{
		int word = x >> 3;
		int bit = x & 7;
		if( value )
		{
			m_data[ y * m_stride + word ] |= 1 << bit;
		}
		else
		{
			m_data[ y * m_stride + word ] &= ~( 1 << bit );
		}
	}

	bool isrowempty( int y )
	{
		for( int i = 0; i < m_stride; ++i )
		{
			if( m_data[ y * m_stride + i ] != 0 )
			{
				return false;
			}
		}
		return true;
	}

	void getbounds( int* pbegin, int* pend )
	{
		int begin = m_height;
		for( int i = 0; i < m_height; ++i )
		{
			if( !isrowempty( i ) )
			{
				begin = i;
				break;
			}
		}
		if( begin == m_height )
		{
			*pbegin = *pend = begin;
			return;
		}
		int end = begin + 1;
		for( int i = m_height - 1; i > begin; --i )
		{
			if( !isrowempty( i ) )
			{
				end = i + 1;
				break;
			}
		}
		*pbegin = begin;
		*pend = end;
	}

	int write( int width, int ybegin, int yend, FILE* output )
	{
		int wordwidth = ( width + 7 ) >> 3;
		for( int row = ybegin; row < yend; ++row )
		{
			fwrite( &m_data[ row * m_stride ], 1, wordwidth, output );
		}
		int size = wordwidth * ( yend - ybegin );
		int tail = size & 15;
		if( tail != 0 )
		{
			static uint8_t zerobuf[ 16 ] = { 0 };
			tail = 16 - tail;
			fwrite( zerobuf, 1, tail, output );
			size = size + tail;
		}
		return size;
	}
};

int datastart;
int datapos;
int rangestart;
int rangeend;

void buildchar(
	HDC hdc, bitmap_t* bm, uint32_t charcode, int valthreshold, FILE* output )
{
	ABC abc;
	if( !GetCharABCWidthsW( hdc, charcode, charcode, &abc ) )
	{
		if( !GetCharWidth32W( hdc, charcode, charcode, ( int* )&abc.abcB ) )
		{
			CheckWinError();
		}
		abc.abcA = abc.abcC = 0;
	}

	wchar_t buffer[ 3 ];
	size_t pointsize;
	utils::encoding::utf16.encode(
		buffer, charcode, sizeof( buffer ), &pointsize );
	TextOutW( hdc, -abc.abcA, 0, buffer, pointsize / 2 );
	bm->clear();
	for( int y = 0; y < TextMetric.tmHeight; ++y )
	{
		int row = TextMetric.tmHeight - y - 1;
		for( int x = 0; x < abc.abcB; ++x )
		{
			COLORREF pixel = GetPixel( hdc, x, y );
			int value =
				int( pixel & 0xff )
				+ int( ( pixel >> 8 ) & 0xff )
				+ int( ( pixel >> 16 ) & 0xff );
			if( value >= valthreshold )
			{
				bm->set( x, row, true );
			}
		}
	}
	int bbegin;
	int bend;
	bm->getbounds( &bbegin, &bend );

	fseek( output, 16 + ( charcode - rangestart ) * 16, SEEK_SET );
	writebeu16( abc.abcB, output ); // width
	writebeu16( bend - bbegin, output ); // height
	writebes16( abc.abcA, output ); // xoffset
	writebes16( bbegin - TextMetric.tmDescent, output ); // yoffset
	writebes16( abc.abcA + abc.abcB + abc.abcC, output ); // advance
	writebeu16( 0, output ); // (reserved)
	writebeu32( 0, output ); // (reserved)
	fseek( output, datapos, SEEK_SET );
	datapos += bm->write( abc.abcB, bbegin, bend, output ); // data
}

int main( int argc, char const** argv )
{
	utils::ConfigSet configset( "NUL",
		convertstr(
			trimpath( GetCommandLineW() ),
			utils::encoding::utf16,
			utils::encoding::utf8 ) );
	utils::String outputpath = configset.string( "output", 0 );
	FILE* output;
	if( outputpath )
	{
		output = fopen( outputpath, "wb" );
		if( !output )
		{
			perror( "" );
			return 0;
		}
	}
	else
	{
		throw std::runtime_error( "output file required" );
	}
	HDC whdc = GetDC( 0 );
	HDC hdc = CreateCompatibleDC( whdc );
	if( !hdc )
	{
		CheckWinError();
	}
	HBITMAP hbitmap = CreateCompatibleBitmap(
		whdc,
		configset.integer( "bitmapsize[1]", 256 ),
		configset.integer( "bitmapsize[2]", 256 ) );
	if( !hbitmap )
	{
		CheckWinError();
	}
	SelectObject( hdc, hbitmap );
	utils::String fontname = configset.string( "face", 0 );
	if( fontname )
	{
		fontname = convertstr(
			fontname,
			utils::encoding::utf8,
			utils::encoding::utf16 );
	}
	HFONT font = CreateFontW(
		configset.integer( "height", 0 ),
		configset.integer( "width", 0 ),
		0,
		0,
		configset.integer( "weight", 400 ),
		configset.boolean( "italic", false ),
		configset.boolean( "underline", false ),
		configset.boolean( "strikeout", false ),
		configset.integer( "charset", DEFAULT_CHARSET ),
		configset.integer( "precision", OUT_DEFAULT_PRECIS ),
		CLIP_DEFAULT_PRECIS,
		configset.integer( "quality", NONANTIALIASED_QUALITY ),
		configset.integer( "pitch", DEFAULT_PITCH )
			| configset.integer( "family", FF_DONTCARE ),
		fontname ? ( wchar_t const* )fontname.getchars() : 0 );
	if( !font ) {
		CheckWinError();
	}
	SetTextColor( hdc, 0xffffff );
	SetBkColor( hdc, 0x000000 );
	if( !SelectObject( hdc, font ) ) {
		CheckWinError();
	}
	GetTextMetrics( hdc, &TextMetric );
	rangestart = configset.integer( "rangestart", 32 );
	rangeend = configset.integer( "rangeend", 128 );
	int valthreshold = configset.integer( "threshold * 3", 128 * 3 );
	writebeu16( TextMetric.tmHeight, output ); // height
	writebeu16( TextMetric.tmDescent, output ); // descent
	writebeu16( TextMetric.tmMaxCharWidth, output ); // maxcharwidth
	writebeu16( 0, output ); // (reserved)
	writebeu32( rangestart, output ); // rangebegin
	writebeu32( rangeend, output ); // rangeend
	bitmap_t* bitmap = bitmap_t::create(
		TextMetric.tmMaxCharWidth, TextMetric.tmHeight );
	datastart = 16 + ( rangeend - rangestart ) * 16;
	datapos = datastart;
	for( uint32_t charcode = rangestart; charcode < rangeend; ++charcode )
	{
		buildchar( hdc, bitmap, charcode, valthreshold, output );
	}
	fseek( output, 6, SEEK_SET );
	writebeu16( ( datapos - datastart + 255 ) / 256, output );
	bitmap->release();
	DeleteObject( font );
	DeleteObject( hbitmap );
	DeleteDC( hdc );
	ReleaseDC( 0, whdc );
	fclose( output );
	return 0;
}
