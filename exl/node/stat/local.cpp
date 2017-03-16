#include <exl/node/stat/local.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	LocalStat::LocalStat()
	{
	}

	LocalStat::LocalStat( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::stat_local );
		ASSERT( s.list.size() == 4 );
		m_targetname = constructidentifier( s[ 2 ] );
		m_value = s[ 3 ] ? constructexpression( s[ 3 ] ) : nullptr;
		m_typev = constructexpression( s[ 4 ] );
	}

	LocalStat::~LocalStat()
	{
	}

	Ref< StringBuilder > LocalStat::getdefstring( size_t depth )
	{
		if( !m_defstring )
		{
			if( m_value )
			{
				m_defstring = StringBuilder::construct(
					"local ", identtostr( m_targetname ),
					" = ", m_value->getdefstring( depth ),
					": ", m_typev->getdefstring( depth ) );
			}
			else
			{
				m_defstring = StringBuilder::construct(
					"local ", identtostr( m_targetname ),
					": ", m_typev->getdefstring( depth ) );
			}
		}
		return m_defstring;
	}
}
