#include <exl/node/expr/name.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	NameExpr::NameExpr()
	{
	}

	NameExpr::NameExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_name );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == NodeHead::name ||
			s[ 2 ].head == NodeHead::fullname );
		ASSERT( s[ 2 ].list.size() >= 1 );
		m_parts.reserve( s[ 2 ].list.size() );
		for( SExpr const& it: s[ 2 ].list )
		{
			m_parts.push_back( constructidentifier( it ) );
		}
		m_fullname = s[ 2 ].head == NodeHead::fullname;
	}

	NameExpr::~NameExpr()
	{
	}

	Ref< StringBuilder > NameExpr::getdefstring( size_t depth )
	{
		if( !m_defstring )
		{
			if( m_fullname )
			{
				m_defstring = StringBuilder::construct(
					".", identtostr( m_parts[ 0 ] ) );
			}
			else
			{
				m_defstring = StringBuilder::construct(
					identtostr( m_parts[ 0 ] ) );
			}
			auto it = m_parts.begin();
			auto eit = m_parts.end();
			++it;
			for( ; it != eit; ++it )
			{
				m_defstring = StringBuilder::construct(
					m_defstring, ".", identtostr( *it ) );
			}
		}
		return m_defstring;
	}
}
