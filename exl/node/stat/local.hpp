#pragma once

#include <exl/symbol/local.hpp>
#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class LocalStat: public Node, public virtual IStatement
	{
	private:
		identifier_t m_targetname;
		Ref< IExpression > m_initexpr;
		Ref< IExpression > m_typev;
		Ref< IContext > m_context;
		Ref< LocalSymbol > m_symbol;
		Ref< ITypeInfo > m_typeinfo;
		Ref< IValue > m_initvalue;
		uint64_t m_register;

	public:
		LocalStat( utils::SExpr const& s );
		~LocalStat();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		virtual void compilereserve( ILBody* body ) override;
		virtual void compileemit( ILBody* body ) override;
	};
}
