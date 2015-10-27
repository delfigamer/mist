#include <cstdio>
#include <cinttypes>

void printhistogram( FILE* f, FILE* fo, uint64_t begin, uint64_t end ) {
	uint64_t count[ 256 ];
	uint8_t byte;
	for( int i = 0; i < 256; ++i ) {
		count[ i ] = 0;
	}
	for( uint64_t i = begin; i < end && !feof( f ); ++i ) {
		fread( &byte, 1, 1, f );
		count[ byte ] += 1;
	}
	for( int i = 0; i < 256; ++i ) {
		fprintf( fo, "[ 0x%.2x ] ( %c ) -> %lli\n", i, ( i >= 0x20 && i <= 0x7e ? i : 0x20 ), count[ i ] );
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
	printhistogram( f, fo, begin, end );
	fclose( f );
	return 0;
}
