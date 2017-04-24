#include <exl/node/stat/local.hpp>
#include <exl/parser/ast.hpp>
#include <exl/construct.hpp>
#include <exl/func.hpp>
#include <exl/error.hpp>
#include <exl/format.hpp>

namespace exl
{
	LocalStat::LocalStat( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::stat_local );
		ASSERT( s.list.size() == 4 );
		m_targetname = constructidentifier( s[ 2 ] );
		m_initexpr = s[ 3 ] ? constructexpression( s[ 3 ] ) : nullptr;
		m_typev = s[ 4 ] ? constructexpression( s[ 4 ] ) : nullptr;
	}

	LocalStat::~LocalStat()
	{
	}

	StringBuilder LocalStat::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "local "_db << identtostr( m_targetname );
		if( m_initexpr )
		{
			defstring << " = "_db << m_initexpr->getdefstring( depth );
		}
		if( m_typev )
		{
			defstring << ": "_db << m_typev->getdefstring( depth );
		}
		return defstring;
	}

	void LocalStat::build( IContext* context )
	{
		m_context = context;
		if( m_typev )
		{
			m_typev->build( context );
			m_typeinfo = gettypevalue( m_typev );
			if( m_initexpr )
			{
				m_initexpr->build( context );
				Ref< IValue > initvalue = m_initexpr->getvalue();
				ASSERT( initvalue );
				m_initvalue = m_typeinfo->cast( initvalue );
				if( !m_initvalue )
				{
					initerror( m_initexpr->gettextrange(), m_typeinfo );
				}
			}
		}
		else if( m_initexpr )
		{
			m_initexpr->build( context );
			m_initvalue = m_initexpr->getvalue();
			ASSERT( m_initvalue );
			m_typeinfo = m_initvalue->getfulltype().type;
		}
		else
		{
			invalidlocalerror( m_textrange );
		}
		m_symbol = Ref< LocalSymbol >::create(
			DefPos{ m_textrange.spos, m_textrange.filename }, m_typeinfo );
		setsymbol( context, m_targetname, m_symbol );
	}

	void LocalStat::compilereserve( ILBody* body )
	{
		m_register = reservereg( body );
		m_symbol->setregister( m_register );
		std::unique_ptr< ILSymbolNote > note( new ILSymbolNote );
			note->type = TokenType::symbolnote;
			note->reg = m_register;
			note->symboltype = SymbolType::symlocal;
			note->fullname = appendident(
				m_context->getnameprefix(), m_targetname );
			note->typeinfo = m_typeinfo->getdefstring( 0 ).combine();
			note->defpos = DefPos{ m_textrange.spos, m_textrange.filename };
		body->notes.push_back( std::move( note ) );
	}

	void LocalStat::compileemit( ILBody* body )
	{
		if( m_initvalue )
		{
			uint64_t reg = m_initvalue->compileread( body );
			appendtoken( body, makeregassignment( m_register, reg ) );
			releasereg( body, reg );
		}
		else
		{
			appendtoken( body, makeregassignment( m_register, 0 ) );
		}
	}
}
