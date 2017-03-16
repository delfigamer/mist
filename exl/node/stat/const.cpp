#include <exl/node/stat/const.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	ConstStat::ConstStat()
	{
	}

	ConstStat::ConstStat( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::stat_const );
		ASSERT( s.list.size() == 3 );
		m_targetname = constructidentifier( s[ 2 ] );
		m_value = constructexpression( s[ 3 ] );
	}

	ConstStat::~ConstStat()
	{
	}

	Ref< StringBuilder > ConstStat::getdefstring( size_t depth )
	{
		if( !m_defstring )
		{
			m_defstring = StringBuilder::construct(
				"const ", identtostr( m_targetname ),
				" = ", m_value->getdefstring( depth ) );
		}
		return m_defstring;
	}
}
