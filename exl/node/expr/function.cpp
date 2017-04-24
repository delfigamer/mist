#include <exl/node/expr/function.hpp>
#include <exl/node/block.hpp>
#include <exl/value/function.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	FunctionExpr::FunctionExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_function );
		ASSERT( s.list.size() == 4 );
		m_argdeflist.emplace( s[ 2 ] );
		if( s[ 3 ] )
		{
			m_rettype = constructexpression( s[ 3 ] );
		}
		m_body = Ref< Block >::create( s[ 4 ] );
	}

	FunctionExpr::~FunctionExpr()
	{
	}

	StringBuilder FunctionExpr::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "function"_db << m_argdeflist->getdefstring( depth );
		if( m_rettype )
		{
			defstring << ": "_db << m_rettype->getdefstring( depth );
		}
		defstring << m_body->getdefstring( depth + 1 )
			<< "\n"_db << lineprefix( depth ) << "end"_db;
		return defstring;
	}

	void FunctionExpr::build( IContext* context )
	{
		Ref< ITypeInfo > rettypeinfo;
		if( m_rettype )
		{
			m_rettype->build( context );
			rettypeinfo = exl::gettypevalue( m_rettype );
		}
		m_argdeflist->build( context );
		m_value = Ref< FunctionValue >::create(
			m_argdeflist->getargs(), rettypeinfo, m_body, context );
	}
}
