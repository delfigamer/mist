#pragma once

#include <exl/types.hpp>
#include <common/ref.hpp>
#include <common.hpp>

namespace exl
{
	Ref< DataBuffer > lineprefix( size_t depth );
	Ref< DataBuffer > inttostr( int number );
	Ref< DataBuffer > textpostostr( TextPos pos );
	Ref< DataBuffer > textrangetostr( TextRange range );
	Ref< DataBuffer > defpostostr( DefPos defpos );
	Ref< DataBuffer > numtostr( double number );
	Ref< DataBuffer > bytestostr( DataBuffer* bytes );
	Ref< DataBuffer > identtostr( identifier_t const& ident );
	Ref< DataBuffer > nametostr( name_t const& name, uint8_t delimiter = '.' );

	template< typename T, typename F >
	StringBuilder sbconcat( T const& elems, size_t depth, StringBuilder delim,
		F const& format )
	{
		if( elems.size() == 0 )
		{
			return nullptr;
		}
		else
		{
			StringBuilder sb = format( elems[ 0 ], depth );
			auto it = elems.begin();
			auto eit = elems.end();
			++it;
			for( ; it != eit; ++it )
			{
				sb << delim << format( *it, depth );
			}
			return sb;
		}
	}

	template< typename T >
	StringBuilder sbconcat( T const& elems, size_t depth, StringBuilder delim )
	{
		return sbconcat(
			elems, depth, delim,
			[]( IObject* e, size_t depth ) -> StringBuilder
			{
				return e->getdefstring( depth );
			} );
	}
}
