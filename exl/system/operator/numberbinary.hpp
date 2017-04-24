#pragma once

#include <exl/system/operator/binary.hpp>
#include <exl/types.hpp>
#include <common/ref.hpp>
#include <common.hpp>

namespace exl
{
	class InvNumberBinary: public InvBinary
	{
	protected:
		virtual double staticvalue( double left, double right ) = 0;

	public:
		InvNumberBinary( ITypeCast* left, ITypeCast* right );
		~InvNumberBinary();

		void constructconstvalue() override;
	};

	class OpNumberBinaryTI: public OpBinaryTI
	{
	public:
		OpNumberBinaryTI();
		~OpNumberBinaryTI();
	};
}
