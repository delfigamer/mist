#include <exl/node/stat/const.hpp>
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

	void ConstStat::compilereserve( ILBody* body )
	{
		m_register = reservereg( body );
		Ref< ITypeInfo > valuetype = m_value->getfulltype().type;
		m_symbol->setregister( m_register );
		std::unique_ptr< ILSymbolNote > note( new ILSymbolNote );
			note->type = TokenType::symbolnote;
			note->reg = m_register;
			note->symboltype = SymbolType::symconst;
			note->fullname = appendident(
				m_context->getnameprefix(), m_targetname );
			note->typeinfo = valuetype->getdefstring( 0 ).combine();
			note->defpos = DefPos{ m_textrange.spos, m_textrange.filename };
		body->notes.push_back( std::move( note ) );
	}

	void ConstStat::compileemit( ILBody* body )
	{
		uint64_t reg = m_value->compileread( body );
		appendtoken( body, makeregassignment( m_register, reg ) );
		releasereg( body, reg );
	}
}
