#include <exl/node/expr/string.hpp>
#include <exl/value/string.hpp>
#include <exl/parser/ast.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	using utils::SExpr;

	StringExpr::StringExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_string );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == SExpr::head_text );
		m_content = s[ 2 ].text;
	}

	StringExpr::~StringExpr()
	{
	}

	StringBuilder StringExpr::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "\""_db << bytestostr( m_content ) << "\""_db;
		return defstring;
	}

	void StringExpr::build( IContext* context )
	{
		m_value = Ref< StringValue >::create( m_content );
	}
}
