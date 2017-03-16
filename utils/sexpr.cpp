#include <utils/sexpr.hpp>
#include <algorithm>
#include <utility>
#include <memory>
#include <cstring>
#include <cstdio>

namespace utils
{
	SENameMap::SENameMap()
	{
	}

	SENameMap::SENameMap(
		std::initializer_list<
			std::pair< uint32_t const, char const* > > il )
		: namemap( il )
	{
	}

	SENameMap::~SENameMap()
	{
	}

	char const* SENameMap::getname( uint32_t index )
	{
		auto it = namemap.find( index );
		if( it != namemap.end() )
		{
			return it->second;
		}
		else
		{
			return "";
		}
	}

	void SENameMap::setname( uint32_t index, char const* name )
	{
		namemap[ index ] = name;
	}



	SEContext::SEContext()
	{
	}

	SEContext::SEContext(
		std::initializer_list<
			std::pair< uint32_t const, uint32_t > > il )
		: propmap( il )
	{
	}

	SEContext::~SEContext()
	{
	}

	uint32_t SEContext::getproperties( uint32_t head )
	{
		auto it = propmap.find( head );
		if( it != propmap.end() )
		{
			return it->second;
		}
		else
		{
			return 0;
		}
	}

	void SEContext::setproperties( uint32_t head, uint32_t value )
	{
		propmap[ head ] = value;
	}



	SExpr::SExpr( uint32_t head )
		: head( head )
	{
		if( head < 0x40000000 )
		{
			new( &list )std::vector< SExpr >();
		}
		else if( head >= 0x70000000 && head < 0x80000000 )
		{
			new( &text )Ref< DataBuffer >();
		}
		else
		{
		}
	}

	SExpr::SExpr( uint32_t head, std::initializer_list< SExpr > il )
		: head( head )
	{
		ASSERT( head < 0x40000000 );
		new( &list )std::vector< SExpr >( il );
	}

	SExpr::SExpr( SExpr const& other )
		: head( other.head )
	{
		if( head < 0x40000000 )
		{
			new( &list )std::vector< SExpr >( other.list );
		}
		else if( head >= 0x70000000 && head < 0x80000000 )
		{
			new( &text )Ref< DataBuffer >( other.text );
		}
		else
		{
			number = other.number;
		}
	}

	SExpr::SExpr( SExpr&& other )
		: head( other.head )
	{
		if( head < 0x40000000 )
		{
			new( &list )std::vector< SExpr >( std::move( other.list ) );
		}
		else if( head >= 0x70000000 && head < 0x80000000 )
		{
			new( &text )Ref< DataBuffer >( std::move( other.text ) );
		}
		else
		{
			number = other.number;
		}
	}

	SExpr::~SExpr()
	{
		if( head < 0x40000000 )
		{
			list.~vector();
		}
		else if( head >= 0x70000000 && head < 0x80000000 )
		{
			text.~Ref();
		}
	}

	SExpr& SExpr::operator=( SExpr const& other )
	{
		if( head < 0x40000000 )
		{
			if( other.head < 0x40000000 )
			{
				list = other.list;
			}
			else if( other.head >= 0x70000000 && other.head < 0x80000000 )
			{
				list.~vector();
				new( &text )Ref< DataBuffer >( other.text );
			}
			else
			{
				list.~vector();
				number = other.number;
			}
		}
		else if( head >= 0x70000000 && head < 0x80000000 )
		{
			if( other.head < 0x40000000 )
			{
				text.~Ref();
				new( &list )std::vector< SExpr >( other.list );
			}
			else if( other.head >= 0x70000000 && other.head < 0x80000000 )
			{
				text = other.text;
			}
			else
			{
				text.~Ref();
				number = other.number;
			}
		}
		else
		{
			if( other.head < 0x40000000 )
			{
				new( &list )std::vector< SExpr >( other.list );
			}
			else if( other.head >= 0x70000000 && other.head < 0x80000000 )
			{
				new( &text )Ref< DataBuffer >( other.text );
			}
			else
			{
				number = other.number;
			}
		}
		head = other.head;
		return *this;
	}

	SExpr& SExpr::operator=( SExpr&& other )
	{
		if( head < 0x40000000 )
		{
			if( other.head < 0x40000000 )
			{
				list = std::move( other.list );
			}
			else if( other.head >= 0x70000000 && other.head < 0x80000000 )
			{
				list.~vector();
				new( &text )Ref< RefObject >( std::move( other.text ) );
			}
			else
			{
				list.~vector();
				number = other.number;
			}
		}
		else if( head >= 0x70000000 && head < 0x80000000 )
		{
			if( other.head < 0x40000000 )
			{
				text.~Ref();
				new( &list )std::vector< SExpr >( std::move( other.list ) );
			}
			else if( other.head >= 0x70000000 && other.head < 0x80000000 )
			{
				text = std::move( other.text );
			}
			else
			{
				text.~Ref();
				number = other.number;
			}
		}
		else
		{
			if( other.head < 0x40000000 )
			{
				new( &list )std::vector< SExpr >( std::move( other.list ) );
			}
			else if( other.head >= 0x70000000 && other.head < 0x80000000 )
			{
				new( &text )Ref< RefObject >( std::move( other.text ) );
			}
			else
			{
				number = other.number;
			}
		}
		head = other.head;
		return *this;
	}

	bool SExpr::assertlist() const
	{
		if( head < 0x40000000 )
		{
			return true;
		}
		else
		{
			fprintf( stderr, "attempt to access the list of sexpr %#" PRIxPTR "\n",
				uintptr_t( this ) );
			dump();
			return false;
		}
	}

	bool SExpr::asserttext() const
	{
		if( head >= 0x70000000 && head < 0x80000000 )
		{
			return true;
		}
		else
		{
			fprintf( stderr, "attempt to access the text of sexpr %#" PRIxPTR "\n",
				uintptr_t( this ) );
			dump();
			return false;
		}
	}

	void SExpr::sethead( uint32_t newhead )
	{
		if( head < 0x40000000 )
		{
			if( newhead < 0x40000000 )
			{
			}
			else if( newhead >= 0x70000000 && newhead < 0x80000000 )
			{
				list.~vector();
				new( &text )Ref< DataBuffer >();
			}
			else
			{
				list.~vector();
			}
		}
		else if( head >= 0x70000000 && head < 0x80000000 )
		{
			if( newhead < 0x40000000 )
			{
				text.~Ref();
				new( &list )std::vector< SExpr >();
			}
			else if( newhead >= 0x70000000 && newhead < 0x80000000 )
			{
			}
			else
			{
				text.~Ref();
			}
		}
		else
		{
			if( newhead < 0x40000000 )
			{
				new( &list )std::vector< SExpr >();
			}
			else if( newhead >= 0x70000000 && newhead < 0x80000000 )
			{
				new( &text )Ref< DataBuffer >();
			}
			else
			{
			}
		}
		head = newhead;
	}

	size_t SExpr::getlength()
	{
		if( assertlist() )
		{
			return list.size();
		}
		else
		{
			return 0;
		}
	}

	void SExpr::setlength( size_t length )
	{
		if( assertlist() )
		{
			list.resize( length );
		}
	}

	SExpr* SExpr::getelements()
	{
		if( assertlist() )
		{
			return list.data() - 1;
		}
		else
		{
			return nullptr;
		}
	}

	DataBuffer* SExpr::gettext()
	{
		if( asserttext() )
		{
			return text;
		}
		else
		{
			return nullptr;
		}
	}

	void SExpr::settext( DataBuffer* newtext )
	{
		if( asserttext() )
		{
			text = newtext;
		}
	}

	void SExpr::shrink_to_fit()
	{
		if( assertlist() )
		{
			list.shrink_to_fit();
		}
	}

	int SExpr::compare( SExpr const& a, SExpr const& b, SENameMap* namemap )
	{
		if( a.head != b.head )
		{
			if( a.head < 0x40000000 )
			{
				if( b.head < 0x40000000 )
				{
					if( namemap )
					{
						char const* aheadname = namemap->getname( a.head );
						char const* bheadname = namemap->getname( b.head );
						return strcmp( aheadname, bheadname );
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return -1;
				}
			}
			else
			{
				if( b.head < 0x40000000 )
				{
					return 1;
				}
				else
				{
					return a.head < b.head ? -1 : 1;
				}
			}
		}
		else if( a.head < 0x40000000 )
		{
			auto ait = a.list.begin();
			auto aeit = a.list.end();
			auto bit = b.list.begin();
			auto beit = b.list.end();
			while( true )
			{
				if( ait != aeit )
				{
					if( bit != beit )
					{
						int d = SExpr::compare( *ait, *bit, namemap );
						if( d != 0 )
						{
							return d;
						}
					}
					else
					{
						return 1;
					}
				}
				else
				{
					if( bit != beit )
					{
						return -1;
					}
					else
					{
						return 0;
					}
				}
				ait++;
				bit++;
			}
		}
		else if( a.head < 0x50000000 )
		{
			return 0;
		}
		else if( a.head < 0x60000000 )
		{
			return a.number < b.number ? -1 : a.number > b.number ? 1 : 0;
		}
		else if( a.head == 0x70000000 )
		{
			if( a.text )
			{
				if( b.text )
				{
					size_t len = a.text->m_length;
					if( b.text->m_length < a.text->m_length )
					{
						len = b.text->m_length;
					}
					int cr = memcmp( a.text->m_data, b.text->m_data, len );
					if( cr == 0 )
					{
						return a.text->m_length < b.text->m_length ? -1 :
							a.text->m_length > b.text->m_length ? 1 : 0;
					}
					else
					{
						return cr;
					}
				}
				else
				{
					return 1;
				}
			}
			else
			{
				if( b.text )
				{
					return -1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		{
			return 0;
		}
	}

	void SExpr::sort( SENameMap* namemap )
	{
		if( assertlist() )
		{
			std::sort( list.begin(), list.end(),
				[ namemap ]( SExpr const& a, SExpr const& b )
				{
					return SExpr::compare( a, b, namemap ) < 0;
				});
		}
	}

	SExpr* SExpr::normalize( SExpr const& s, SEContext* cx )
	{
		if( s.assertlist() )
		{
			bool isflat = cx->getproperties( s.head ) & property_flat;
			std::unique_ptr< SExpr > ret( new SExpr( s.head ) );
			for( SExpr const& item: s.list )
			{
				if( item.head >= 0x40000000 )
				{
					ret->list.push_back( item );
				}
				else
				{
					std::unique_ptr< SExpr > nitem( SExpr::normalize( item, cx ) );
					if( isflat && nitem->head == s.head )
					{
						for( SExpr& subitem: nitem->list )
						{
							ret->list.push_back( std::move( subitem ) );
						}
					}
					else
					{
						ret->list.push_back( std::move( *nitem ) );
					}
				}
			}
			return ret.release();
		}
		else
		{
			return nullptr;
		}
	}

	void dumpqformatchar( FILE* fout, unsigned char c, unsigned char next )
	{
		if( c == '\n' || c == '"' || c == '\\' )
		{
			fputc( '\\', fout );
			fputc( c, fout );
		}
		else if( c < 32 )
		{
			if( next >= '0' && next <= '9' )
			{
				fprintf( fout, "\\%.3i", c );
			}
			else
			{
				fprintf( fout, "\\%i", c );
			}
		}
		else
		{
			fputc( c, fout );
		}
	}

	void dumpqformat( FILE* fout, void const* vstr )
	{
		unsigned char const* str = ( unsigned char const* )vstr;
		fputc( '\"', fout );
		while( str[ 0 ] )
		{
			dumpqformatchar( fout, str[ 0 ], str[ 1 ] );
			str += 1;
		}
		fputc( '\"', fout );
	}

	void dumpqformat( FILE* fout, void const* vstr, size_t len )
	{
		unsigned char const* str = ( unsigned char const* )vstr;
		fputc( '\"', fout );
		while( len > 1 )
		{
			dumpqformatchar( fout, str[ 0 ], str[ 1 ] );
			str += 1;
			len -= 1;
		}
		if( len == 1 )
		{
			dumpqformatchar( fout, str[ 0 ], 0 );
		}
		fputc( '\"', fout );
	}

	void SExpr::dump( SENameMap* namemap, size_t depth ) const
	{
		for( size_t i = 0; i < depth; ++i )
		{
			fprintf( stderr, "\t" );
		}
		if( head < 0x40000000 )
		{
			if( head != 0 )
			{
				if( namemap )
				{
					char const* name = namemap->getname( head );
					fprintf( stderr, "{[0]=" );
					dumpqformat( stderr, name );
					fprintf( stderr, ",\n" );
				}
				else
				{
					fprintf( stderr, "{[0]=%#" PRIx32 ",\n", head );
				}
			}
			else
			{
				fprintf( stderr, "{" );
			}
			for( SExpr const& elem: list )
			{
				elem.dump( namemap, depth + 1 );
			}
			for( size_t i = 0; i < depth; ++i )
			{
				fprintf( stderr, "\t" );
			}
			fprintf( stderr, "}%s\n", depth == 0 ? "" : "," );
		}
		else if( head == 0x40000000 )
		{
			fprintf( stderr, "false%s\n", depth == 0 ? "" : "," );
		}
		else if( head == 0x40000001 )
		{
			fprintf( stderr, "true%s\n", depth == 0 ? "" : "," );
		}
		else if( head == 0x50000000 )
		{
			fprintf( stderr, "%lg%s\n", number, depth == 0 ? "" : "," );
		}
		else if( head == 0x70000000 )
		{
			if( text )
			{
				dumpqformat( stderr, text->m_data, text->m_length );
				fprintf( stderr, "%s\n", depth == 0 ? "" : "," );
			}
			else
			{
				fprintf( stderr, "\"\"%s\n", depth == 0 ? "" : "," );
			}
		}
		else
		{
			fprintf( stderr,
				"sexpr{head = %" PRIu32 ", number = %lg}%s\n",
				head, number, depth == 0 ? "" : "," );
		}
	}

	SExpr SExpr::atom()
	{
		return SExpr( 0x40000000 );
	}

	SExpr SExpr::atom( int number )
	{
		SExpr ret( 0x50000000 );
		ret.number = number;
		return ret;
	}

	SExpr SExpr::atom( double number )
	{
		SExpr ret( 0x50000000 );
		ret.number = number;
		return ret;
	}

	SExpr SExpr::atom( bool boolean )
	{
		SExpr ret( boolean ? 0x40000001 : 0x40000000 );
		return ret;
	}

	SExpr SExpr::atom( Ref< DataBuffer > const& text )
	{
		SExpr ret( 0x70000000 );
		ret.text = text;
		return ret;
	}

	SExpr SExpr::atom( Ref< DataBuffer >&& text )
	{
		SExpr ret( 0x70000000 );
		ret.text = std::move( text );
		return ret;
	}
}
