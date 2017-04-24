#include <exl/node/stat/function.hpp>
#include <exl/node/block.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	FunctionStat::FunctionStat( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::stat_function );
		ASSERT( s.list.size() == 5 );
		ASSERT( s[ 2 ].head == NodeHead::name ||
			s[ 2 ].head == NodeHead::fullname );
		ASSERT( s[ 2 ].list.size() >= 1 );
		m_name = apply( s[ 2 ].list, constructidentifier );
		m_fullname = s[ 2 ].head == NodeHead::fullname;
		m_argdeflist.emplace( s[ 3 ] );
		if( s[ 4 ] )
		{
			m_rettype = constructexpression( s[ 4 ] );
		}
		m_body = Ref< Block >::create( s[ 5 ] );
	}

	FunctionStat::~FunctionStat()
	{
	}

	StringBuilder FunctionStat::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "function "_db;
		if( m_fullname )
		{
			defstring << "."_db;
		}
		defstring << nametostr( m_name );
		defstring << m_argdeflist->getdefstring( depth );
		if( m_rettype )
		{
			defstring << ": "_db << m_rettype->getdefstring( depth );
		}
		defstring << m_body->getdefstring( depth + 1 )
			<< "\n"_db << lineprefix( depth ) << "end"_db;
		return defstring;
	}

	void FunctionStat::build( IContext* context )
	{
	}

	void FunctionStat::compilereserve( ILBody* body )
	{
	}

	void FunctionStat::compileemit( ILBody* body )
	{
	}
}
