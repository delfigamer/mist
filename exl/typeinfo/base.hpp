#pragma once

#include <exl/object.hpp>
#include <exl/types.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class TypeInfo: public Object, virtual public ITypeInfo
	{
	public:
		TypeInfo();
		~TypeInfo();

		virtual Ref< ITypeCast > cast( IValue* source ) override;
		virtual Ref< IInvocation > invoke(
			IValue* target,
			std::vector< Ref< IValue > > const& args ) override;
	};
}
