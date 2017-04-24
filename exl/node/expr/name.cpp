#include <exl/node/expr/name.hpp>
#include <exl/value/reference.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/error.hpp>
#include <exl/format.hpp>

namespace exl
{
	using utils::SExpr;

	NameExpr::NameExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_name );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == NodeHead::name ||
			s[ 2 ].head == NodeHead::fullname );
		ASSERT( s[ 2 ].list.size() >= 1 );
		m_name = apply( s[ 2 ].list, constructidentifier );
		m_fullname = s[ 2 ].head == NodeHead::fullname;
	}

	NameExpr::~NameExpr()
	{
	}

	StringBuilder NameExpr::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		if( m_fullname )
		{
			defstring << "."_db;
		}
		defstring << nametostr( m_name );
		return defstring;
	}

	void NameExpr::build( IContext* context )
	{
		if( !m_fullname && m_name.size() == 1 )
		{
			m_target = context->getsymbol( m_name[ 0 ] );
			if( !m_target )
			{
				undefinedsymbolerror( gettextrange(), m_name[ 0 ] );
			}
		}
		else
		{
			nyierror( gettextrange(), __FILE__, __LINE__ );
		}
		m_value = Ref< ReferenceValue >::create( m_target );
	}
}
