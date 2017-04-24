#pragma once

#include <exl/symbol/local.hpp>
#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	class ArgDefNode: public Node
	{
	private:
		bool m_readable;
		bool m_writable;
		bool m_named;
		identifier_t m_targetname;
		Ref< IExpression > m_initexpr;
		Ref< IExpression > m_typev;
		FullType m_fulltype;
		Ref< IValue > m_initvalue;

	public:
		ArgDefNode();
		ArgDefNode( utils::SExpr const& s );
		~ArgDefNode();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		ArgDef getargdef();
	};
}
