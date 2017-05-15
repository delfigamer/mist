#include <exl/node/stat/const.hpp>
#include <exl/il/types.hpp>
#include <exl/parser/ast.hpp>
#include <exl/construct.hpp>
#include <exl/func.hpp>
#include <exl/error.hpp>
#include <exl/format.hpp>

namespace exl
{
	ConstStat::ConstStat( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::stat_const );
		ASSERT( s.list.size() == 3 );
		m_targetname = constructidentifier( s[ 2 ] );
		m_valueexpr = constructexpression( s[ 3 ] );
	}

	ConstStat::~ConstStat()
	{
	}

	StringBuilder ConstStat::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "const "_db << identtostr( m_targetname )
			<< " = "_db << m_valueexpr->getdefstring( depth );
		return defstring;
	}

	void ConstStat::build( IContext* context )
	{
		m_context = context;
		m_valueexpr->build( context );
		m_value = m_valueexpr->getvalue();
		ASSERT( m_value );
		Ref< IConstValue > constvalue = m_value->getconstvalue();
		if( !constvalue )
		{
			consterror( m_valueexpr->gettextrange() );
		}
		m_symbol = Ref< ConstSymbol >::create(
			DefPos{ m_textrange.spos, m_textrange.filename }, constvalue );
		setsymbol( context, m_targetname, m_symbol );
	}

	void ConstStat::compile( ILBody* body )
	{
		ILValue ilvalue;
		m_value->compileread( body, ilvalue );
		m_symbol->setvalue( ilvalue );
	}
}
