#pragma once

#include <exl/typeinfo/base.hpp>
#include <exl/node/argdeflist.hpp>
#include <exl/types.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class FunctionTI: public TypeInfo, virtual public ITypeInfo
	{
	private:
		std::vector< FullType > m_argdefs;
		Ref< ITypeInfo > m_rettypeinfo;

	public:
		FunctionTI(
			std::vector< ArgDef > const& argdeflist, ITypeInfo* rettypeinfo );
		~FunctionTI();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual Ref< IInvocation > invoke(
			IValue* target,
			std::vector< Ref< IValue > > const& args ) override;
		std::vector< FullType >& argdefs();
	};
}
