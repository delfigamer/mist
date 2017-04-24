#include <exl/node/expr/functiontype.hpp>
#include <exl/value/functiontype.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	FunctionTypeExpr::FunctionTypeExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_functiontype );
		ASSERT( s.list.size() == 3 );
		m_argdeflist.emplace( s[ 2 ] );
		if( s[ 3 ] )
		{
			m_rettype = constructexpression( s[ 3 ] );
		}
	}

	FunctionTypeExpr::~FunctionTypeExpr()
	{
	}

	StringBuilder FunctionTypeExpr::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "type function"_db << m_argdeflist->getdefstring( depth );
		if( m_rettype )
		{
			defstring << ": "_db << m_rettype->getdefstring( depth );
		}
		return defstring;
	}

	void FunctionTypeExpr::build( IContext* context )
	{
		m_argdeflist->build( context );
		if( m_rettype )
		{
			m_rettype->build( context );
			m_rettypeinfo = exl::gettypevalue( m_rettype );
		}
		m_value = Ref< FunctionTypeValue >::create(
			m_argdeflist->getargs(), m_rettypeinfo );
	}
}
