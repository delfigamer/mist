#include <random>
#include <vector>
#include <ctime>
#include <cmath>
#include <cstdio>

std::vector< int > primes;
int primes_end = 2;

void fillprimes( int newend )
{
	for( int val = primes_end; val < newend; ++val )
	{
		bool isprime = true;
		for( auto it: primes )
		{
			if( val % it == 0 )
			{
				isprime = false;
				break;
			}
		}
		if( isprime )
		{
			primes.push_back( val );
		}
		if( val % 10000 == 0 )
		{
			printf( "\t\t%i\n", val );
		}
	}
	primes_end = newend;
}

int divprod( int target )
{
	int result = 1;
	int tend = ceil( sqrt( target ) );
	// fillprimes( tend + 1 );
	// for( auto it : primes )
	// {
		// if( it > tend )
		// {
			// break;
		// }
		// else if( target % it == 0 )
		// {
			// result *= it;
		// }
	// }
	for( int i = 2; i <= tend; ++i )
	{
		if( target % i != 0 )
		{
			continue;
		}
		bool isprime = true;
		for( int j = 2; j < i; ++j )
		{
			if( i % j == 0 )
			{
				isprime = false;
				break;
			}
		}
		if( isprime )
		{
			result *= i;
		}
	}
	if( result == 1 )
	{
		result = target;
	}
	return result;
}

int main( int argc, char** argv )
{
	static int const test[] = {
		1,
		2*2*2*2*5*5*5*7*7*11,
		127*127,
		1039,
	};
	for( int i = 0; i < int( sizeof( test ) / sizeof( test[ 0 ] ) ); ++i )
	{
		printf( "%i\t%i\n", test[ i ], divprod( test[ i ] ) );
	}
	int volatile* res = new int[ 100000 ];
	std::mt19937 rand( 123 );
	clock_t t = clock();
	for( int i = 0; i < 100000; ++i )
	{
		int val = rand() & 0xffffff;
		int dp = divprod( val );
		res[ i ] = dp;
	}
	printf( "%f", float( clock() - t ) / CLOCKS_PER_SEC );
	delete[] res;
	return 0;
}
