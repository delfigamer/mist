#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class Block: public Node, public virtual IBlock
	{
	private:
		std::vector< Ref< IStatement > > m_nodes;

	public:
		Block();
		Block( utils::SExpr const& s );
		~Block();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		virtual void compilereserve( ILBody* body ) override;
		virtual void compileemit( ILBody* body ) override;
	};
}
