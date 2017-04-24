#include <exl/node/argdef.hpp>
#include <exl/parser/ast.hpp>
#include <exl/construct.hpp>
#include <exl/func.hpp>
#include <exl/format.hpp>

namespace exl
{
	using utils::SExpr;

	ArgDefNode::ArgDefNode()
	{
	}

	ArgDefNode::ArgDefNode( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::argdef );
		ASSERT( s.list.size() == 6 );
		m_writable = s[ 2 ];
		m_readable = s[ 3 ];
		m_named = s[ 4 ];
		if( m_named )
		{
			m_targetname = constructidentifier( s[ 4 ] );
		}
		m_initexpr = s[ 5 ] ? constructexpression( s[ 5 ] ) : nullptr;
		m_typev = constructexpression( s[ 6 ] );
	}

	ArgDefNode::~ArgDefNode()
	{
	}

	StringBuilder ArgDefNode::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		if( m_readable )
		{
			if( m_writable )
			{
				defstring << "inout"_db;
			}
			else
			{
			}
		}
		else
		{
			if( m_writable )
			{
				defstring << "out"_db;
			}
			else
			{
				defstring << "void"_db;
				return defstring;
			}
		}
		if( m_named )
		{
			defstring
				<< ( defstring ? " "_db : nullptr ) << identtostr( m_targetname );
		}
		if( m_initexpr )
		{
			defstring
				<< ( defstring ? " = "_db : "= "_db )
				<< m_initexpr->getdefstring( depth );
		}
		defstring << ": "_db << m_typev->getdefstring( depth );
		return defstring;
	}

	void ArgDefNode::build( IContext* context )
	{
		if( m_initexpr )
		{
			m_initexpr->build( context );
			m_initvalue = m_initexpr->getvalue();
		}
		m_typev->build( context );
		m_fulltype = FullType{ gettypevalue( m_typev ), m_readable, m_writable };
	}

	ArgDef ArgDefNode::getargdef()
	{
		return ArgDef{
			DefPos{ m_textrange.spos, m_textrange.filename },
			m_named, m_targetname, m_fulltype, m_initvalue };
	}
}
