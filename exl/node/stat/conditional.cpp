#include <exl/node/stat/conditional.hpp>
#include <exl/node/block.hpp>
#include <exl/context.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/error.hpp>
#include <exl/format.hpp>
#include <stack>

namespace exl
{
	ConditionalStat::ConditionalStat( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::stat_conditional );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == 0 );
#if MIST_DEBUG
		bool defaultemitted = false;
#endif
		m_branches = apply(
			s[ 2 ].list,
			[ & ]( utils::SExpr const& bs )
			{
				ASSERT( bs.head == NodeHead::branch );
				ASSERT( bs.list.size() == 2 );
#if MIST_DEBUG
				if( !bs[ 1 ] )
				{
					ASSERT( !defaultemitted );
					defaultemitted = true;
				}
#endif
				branch_t ret;
				ret.condexpr = bs[ 1 ] ? constructexpression( bs[ 1 ] ) : nullptr;
				ret.block = Ref< Block >::create( bs[ 2 ] );
				return ret;
			} );
	}

	ConditionalStat::~ConditionalStat()
	{
	}

	StringBuilder ConditionalStat::getdefstring( size_t depth )
	{
		if( m_branches.empty() )
		{
			return nullptr;
		}
		StringBuilder defstring;
		defstring
			<< sbconcat( m_branches, depth, "else"_db,
				[]( branch_t const& branch, size_t depth )
				{
					StringBuilder ret;
					if( branch.condexpr )
					{
						ret
							<< "if "_db
							<< branch.condexpr->getdefstring( depth )
							<< " then"_db;
					}
					ret
						<< branch.block->getdefstring( depth + 1 )
						<< "\n"_db << lineprefix( depth );
					return ret;
				} )
			<< "end"_db;
		return defstring;
	}

	void ConditionalStat::build( IContext* context )
	{
		for( auto& branch: m_branches )
		{
			name_t nameprefix = appendident(
				context->getnameprefix(),
				anonymousident( branch.block->gettextrange() ) );
			branch.context = Ref< Context >::create( nameprefix, context );
			if( branch.condexpr )
			{
				branch.condexpr->build( branch.context );
				branch.condvalue = branch.condexpr->getvalue();
				ASSERT( branch.condvalue );
				if( !branch.condvalue->getfulltype().readable )
				{
					readableerror( branch.condexpr->gettextrange() );
				}
			}
			branch.block->build( branch.context );
		}
	}

	void ConditionalStat::compilereserve( ILBody* body )
	{
		for( auto& branch: m_branches )
		{
			branch.block->compilereserve( body );
		}
	}

	void ConditionalStat::compileemit( ILBody* body )
	{
		ASSERT( !m_branches.empty() );
		std::stack< ILBlock* > paths;
		for( auto& branch: m_branches )
		{
			ILBlock* startblock = body->currentblock;
			std::unique_ptr< ILValue > cond;
			if( branch.condvalue )
			{
				uint64_t condreg = branch.condvalue->compileread( body );
				releasereg( body, condreg );
				createblock( body );
				startblock->branches.push_back(
					ILBranch{ makeregvalue( condreg ), body->currentblock } );
				branch.block->compileemit( body );
				paths.push( body->currentblock );
			}
			else
			{
				branch.block->compileemit( body );
			}
			createblock( body );
			startblock->branches.push_back(
				ILBranch{ nullptr, body->currentblock } );
		}
		while( !paths.empty() )
		{
			paths.top()->branches.push_back(
				ILBranch{ nullptr, body->currentblock } );
			paths.pop();
		}
	}
}
