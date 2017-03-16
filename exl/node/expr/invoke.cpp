#include <exl/node/expr/invoke.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	InvokeExpr::InvokeExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_invoke );
		ASSERT( s.list.size() == 3 );
		ASSERT( s[ 3 ].head == 0 );
		m_target = constructexpression( s[ 2 ] );
		m_args.reserve( s[ 3 ].list.size() );
		for( SExpr const& it: s[ 3 ].list )
		{
			Ref< IExpression > node = constructexpression( it );
			m_args.push_back( std::move( node ) );
		}
	}

	InvokeExpr::~InvokeExpr()
	{
	}

	Ref< StringBuilder > InvokeExpr::getdefstring( size_t depth )
	{
		if( !m_defstring )
		{
			m_defstring = m_target->getdefstring( depth );
			if( m_args.size() == 0 )
			{
				m_defstring = StringBuilder::construct(
					m_defstring, "[]" );
			}
			else if( m_args.size() == 1 )
			{
				m_defstring = StringBuilder::construct(
					m_defstring, "[", m_args[ 0 ]->getdefstring( depth ), "]" );
			}
			else
			{
				m_defstring = StringBuilder::construct(
					m_defstring, "[", m_args[ 0 ]->getdefstring( depth ) );
				auto it = m_args.begin();
				auto eit = m_args.end();
				++it;
				for( ; it != eit; ++it )
				{
					m_defstring = StringBuilder::construct(
						m_defstring, ", ", ( *it )->getdefstring( depth ) );
				}
				m_defstring = StringBuilder::construct(
					m_defstring, "]" );
			}
		}
		return m_defstring;
	}
}
