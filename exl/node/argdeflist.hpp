#pragma once

#include <exl/node/base.hpp>
#include <exl/node/argdef.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class ArgDefList: public Node
	{
	private:
		std::vector< Ref< ArgDefNode > > m_argnodes;
		std::vector< ArgDef > m_argdefs;

	public:
		ArgDefList();
		ArgDefList( utils::SExpr const& s );
		~ArgDefList();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		std::vector< ArgDef > const& getargs();
	};
}
