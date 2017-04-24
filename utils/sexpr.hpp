#pragma once

#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>
#include <map>
#include <vector>
#include <initializer_list>
#include <tuple>

// Head namespace:
//          0 .. 0x3fffffff .list   compound expressions
//          0: headless list
// 0x40000000 .. 0x4fffffff         stateless atoms
//          0: nil/false
//          1: true
// 0x50000000 .. 0x5fffffff .number
//          0: number
// 0x60000000 .. 0x6fffffff         reserved
// 0x70000000 .. 0x7fffffff .text
//          0: string
// 0x80000000 .. 0x8fffffff         generic patterns
// 0x90000000 .. 0xffffffff         reserved

namespace utils
{
	R_CLASS( name = sexprnamemap )
	class SENameMap: public RefObject
	{
	private:
		std::map< uint32_t, char const* > namemap;

	public:
		SENameMap();
		SENameMap(
			std::initializer_list<
				std::pair< uint32_t const, char const* > > il );
		SENameMap( SENameMap const& other ) = delete;
		~SENameMap();
		SENameMap& operator=( SENameMap const& other ) = delete;

		R_METHOD( stringwrap ) char const* getname( uint32_t index );
		R_METHOD() void setname( uint32_t index, char const* name );
		R_METHOD() static SENameMap* create()
		{
			return new SENameMap;
		}
	};

	enum
	{
		property_flat = 1,
	};

	R_CLASS( name = sexprcontext )
	class SEContext: public RefObject
	{
	private:
		std::map< uint32_t, uint32_t > propmap;

	public:
		SEContext();
		SEContext(
			std::initializer_list<
				std::pair< uint32_t const, uint32_t > > il );
		SEContext( SEContext const& other ) = delete;
		~SEContext();
		SEContext& operator=( SEContext const& other ) = delete;

		R_METHOD() uint32_t getproperties( uint32_t head );
		R_METHOD() void setproperties( uint32_t head, uint32_t value );
		R_METHOD() static SEContext* create()
		{
			return new SEContext;
		}
	};

/*
R_EMIT( target = ffi_start, typename = ::utils::SExpr | struct sexpr )
	struct sexpr {
		union {
			double number;
			void* data[ 3 ];
		};
		uint32_t head;
	};
R_END()

R_EMIT( target = hpp_beforeclasses )
	namespace utils {struct SExpr;}
R_END()
*/

	struct SExpr
	{
		enum
		{
			head_nil = 0x40000000,
			head_true = 0x40000001,
			head_number = 0x50000000,
			head_text = 0x70000000,
		};

		union
		{
			std::vector< SExpr > list;
			Ref< DataBuffer > text;
			double number;
			void* data[ 3 ];
		};
		uint32_t head;

		explicit SExpr( uint32_t head = 0x40000000 );
		SExpr( uint32_t head, std::initializer_list< SExpr > il );
		SExpr( SExpr const& other );
		SExpr( SExpr&& other );
		~SExpr();
		SExpr& operator=( SExpr const& other );
		SExpr& operator=( SExpr&& other );
		bool assertlist() const;
		bool asserttext() const;
		void sethead( uint32_t newhead );
		size_t getlength();
		void setlength( size_t length );
		SExpr* getelements();
		DataBuffer* gettext();
		void settext( DataBuffer* newtext );
		template< typename... Ts >
			SExpr& append( Ts&&... args );
		void shrink_to_fit();
		static int compare( SExpr const& a, SExpr const& b, SENameMap* namemap );
		void sort( SENameMap* namemap );
		static SExpr* normalize( SExpr const& s, SEContext* cx );
		void dump( SENameMap* namemap = nullptr, size_t depth = 0 ) const;

		static SExpr atom();
		static SExpr atom( int number );
		static SExpr atom( double number );
		static SExpr atom( bool boolean );
		static SExpr atom( Ref< DataBuffer > const& text );
		static SExpr atom( Ref< DataBuffer >&& text );

		operator bool() const
		{
			return head != 0x40000000;
		}

		SExpr& operator[]( ptrdiff_t index )
		{
			return list[ index-1 ];
		}

		SExpr const& operator[]( size_t index ) const
		{
			return list[ index-1 ];
		}
	};

	template< typename... Ts >
	SExpr& SExpr::append( Ts&&... args )
	{
		if( assertlist() )
		{
			list.emplace_back( std::forward< Ts >( args )... );
			return list.back();
		}
		else
		{
			return *( SExpr* )nullptr;
		}
	}

	R_CLASS( name = sexpr )
	struct SExprMethods
	{
		R_METHOD( name = new, gc = methodlist.sexpr_delete )
			static SExpr* create( uint32_t head )
		{
			return new SExpr( head );
		}

		R_METHOD( gc = methodlist.sexpr_delete ) static SExpr* clone( SExpr* s )
		{
			return new SExpr( *s );
		}

		R_METHOD( gc = methodlist.sexpr_delete ) static SExpr* detach( SExpr* s )
		{
			return new SExpr( std::move( *s ) );
		}

		R_METHOD( name = delete, noluamethod ) static void destroy( SExpr* s )
		{
			delete s;
		}

		R_METHOD() static void assign( SExpr* target, SExpr* source )
		{
			*target = *source;
		}

		R_METHOD() static void move( SExpr* target, SExpr* source )
		{
			*target = std::move( *source );
		}

		R_METHOD() static void sethead( SExpr* s, uint32_t newhead )
		{
			s->sethead( newhead );
		}

		R_METHOD() static size_t getlength( SExpr* s )
		{
			return s->getlength();
		}

		R_METHOD() static void setlength( SExpr* s, size_t length )
		{
			s->setlength( length );
		}

		R_METHOD() static DataBuffer* gettext( SExpr* s )
		{
			DataBuffer* text = s->gettext();
			::addref( text );
			return text;
		}

		R_METHOD() static void settext( SExpr* s, DataBuffer* newtext )
		{
			s->settext( newtext );
		}

		R_METHOD() static SExpr* getelements( SExpr* s )
		{
			return s->getelements();
		}

		R_METHOD() static int compare(
			SExpr* a, SExpr* b, SENameMap* namemap )
		{
			return SExpr::compare( *a, *b, namemap );
		}

		R_METHOD() static void sort( SExpr* s, SENameMap* namemap )
		{
			s->sort( namemap );
		}

		R_METHOD( gc = methodlist.sexpr_delete )
			static SExpr* normalize( SExpr* s, SEContext* cx )
		{
			return SExpr::normalize( *s, cx );
		}

		R_METHOD() static void dump( SExpr* s, SENameMap* namemap )
		{
			s->dump( namemap );
		}
	};
}
