#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class UnitFile: public Node, public virtual IFileNode
	{
	private:
		Ref< IBlock > m_body;
		ILModule m_ilfile;

	public:
		UnitFile( utils::SExpr const& s );
		~UnitFile();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		virtual ILModule& compile() override;
	};
}
