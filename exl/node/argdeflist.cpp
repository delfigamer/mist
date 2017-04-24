#include <exl/node/argdeflist.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	ArgDefList::ArgDefList()
	{
	}

	ArgDefList::ArgDefList( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::argdeflist );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == 0 );
		m_argnodes = apply(
			s[ 2 ].list, Ref< ArgDefNode >::create< utils::SExpr const& > );
	}

	ArgDefList::~ArgDefList()
	{
	}

	StringBuilder ArgDefList::getdefstring( size_t depth )
	{
		return StringBuilder()
			<< "["_db << sbconcat( m_argnodes, depth, ", "_db ) << "]"_db;
	}

	void ArgDefList::build( IContext* context )
	{
		for( auto& argnode: m_argnodes )
		{
			argnode->build( context );
		}
		m_argdefs = apply( m_argnodes, &ArgDefNode::getargdef );
	}

	std::vector< ArgDef > const& ArgDefList::getargs()
	{
		return m_argdefs;
	}
}
