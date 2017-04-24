#pragma once

#include <exl/typeinfo/base.hpp>
#include <exl/value/base.hpp>
#include <exl/types.hpp>
#include <common/ref.hpp>
#include <common.hpp>

namespace exl
{
	class InvBinary: public Value, public virtual IInvocation
	{
	protected:
		Ref< ITypeCast > m_left;
		Ref< ITypeCast > m_right;
		Ref< IConstValue > m_constvalue;
		TokenType m_operatortype;

	public:
		InvBinary( ITypeCast* left, ITypeCast* right );
		~InvBinary();

		virtual Ref< IConstValue > getconstvalue() override;
		virtual uint64_t compileread( ILBody* body ) override;
		virtual void constructconstvalue() = 0;
	};

	class OpBinaryTI: public TypeInfo
	{
	protected:
		virtual Ref< InvBinary > createinvocation(
			ITypeCast* left, ITypeCast* right ) = 0;

	protected:
		Ref< ITypeInfo > m_argtypeinfo;

	public:
		OpBinaryTI();
		~OpBinaryTI();

		virtual Ref< IInvocation > invoke(
			IValue* target,
			std::vector< Ref< IValue > > const& args ) override;
	};
}
