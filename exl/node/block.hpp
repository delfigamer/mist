#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>
#include <cinttypes>

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

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;
	};
}
