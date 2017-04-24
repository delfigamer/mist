#include <cstdio>

void printlineheader( FILE* fo )
{
	fprintf( fo, "             0 1 2 3  4 5 6 7  8 9 a b  c d e f\n" );
}

void printline( FILE* fo, uint8_t const* buffer, uint64_t begin, int length )
{
	fprintf( fo, "%10llx:", begin & 0xfffffffffffffff0LL );
	int loffset = begin & 0xf;
	buffer -= loffset;
	length += loffset;
	for( int i = 0; i < loffset; ++i ) {
		fprintf( fo, "%s  ", i % 4 == 0 ? " " : "" );
	}
	for( int i = loffset; i < length; ++i ) {
		fprintf( fo, "%s%.2x", i % 4 == 0 ? " " : "", buffer[ i ] );
	}
	for( int i = length; i < 16; ++i ) {
		fprintf( fo, "%s  ", i % 4 == 0 ? " " : "" );
	}
	fprintf( fo, " " );
	for( int i = 0; i < loffset; ++i ) {
		fprintf( fo, " " );
	}
	for( int i = loffset; i < length; ++i ) {
		if( ( buffer[ i ] >= 32 ) && ( buffer[ i ] < 127 ) ) {
			fprintf( fo, "%c", buffer[ i ] );
		} else {
			fprintf( fo, " " );
		}
	}
	for( int i = length; i < 16; ++i ) {
		fprintf( fo, " " );
	}
	fprintf( fo, "\n" );
}

uint64_t printpage( FILE* f, FILE* fo, uint64_t begin, uint64_t end ) {
	uint64_t pagebegin = begin & 0xffffffffffffff00LL;
	uint64_t pageend = pagebegin + 0x100;
	if( end > pageend ) {
		end = pageend;
	}
	int pageoffset = begin & 0xff;
	int pagelength = end - pagebegin;
	uint8_t buffer[ 0x100 ];
	pagelength = fread( buffer + pageoffset, 1, pagelength - pageoffset, f ) + pageoffset;
	printlineheader( fo );
	if( ( pageoffset & 0xf ) != 0 ) {
		int fl = 0x10 - pageoffset & 0xf;
		if( pageoffset + fl > pagelength ) {
			printline( fo, buffer + pageoffset, begin, end - begin );
			return end;
		}
		printline( fo, buffer + pageoffset, begin, fl );
		begin += fl;
		pageoffset += fl;
	}
	while( pageoffset + 16 <= pagelength ) {
		printline( fo, buffer + pageoffset, begin, 16 );
		begin += 16;
		pageoffset += 16;
	}
	if( pageoffset < pagelength ) {
		printline( fo, buffer + pageoffset, begin, pagelength - pageoffset );
		end = begin + pagelength - pageoffset;
	}
	return end;
}

void printrange( FILE* f, FILE* fo, uint64_t begin, uint64_t end ) {
	uint64_t fulllen = end - begin;
	int dc = 80;
	while( !feof( f ) && begin < end ) {
		begin = printpage( f, fo, begin, end );
		int dc2 = ( uint64_t )80 * ( end - begin ) / fulllen;
		while( dc2 < dc ) {
			printf( "." );
			--dc;
		}
	}
}

int main( int argc, char const** argv ) {
	if( argc < 3 ) {
		printf( "Usage: hexdump <file> [[<begin> = 0] <end> = file_size] <output>\n" );
		return 0;
	}
	char const* path = argv[ 1 ];
	FILE* f = fopen( path, "rb" );
	FILE* fo;
	uint64_t begin, end;
	if( argc == 3 ) {
		begin = 0;
		fseeko64( f, 0, SEEK_END );
		fgetpos( f, ( fpos_t* )&end );
		fo = fopen( argv[ 2 ], "w" );
	} else if( argc == 4 ) {
		begin = 0;
		sscanf( argv[ 2 ], "%lli", &end );
		fo = fopen( argv[ 3 ], "w" );
	} else {
		sscanf( argv[ 2 ], "%lli", &begin );
		sscanf( argv[ 3 ], "%lli", &end );
		fo = fopen( argv[ 4 ], "w" );
	}
	fsetpos( f, ( fpos_t* )&begin );
	printrange( f, fo, begin, end );
	fclose( f );
	return 0;
}
