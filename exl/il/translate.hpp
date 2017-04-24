#pragma once

#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	extern Ref< utils::SENameMap > ILNameMap;
	extern Ref< utils::SEContext > ILContext;

	utils::SExpr translatemodule( ILModule const& module );

	R_CLASS( name = exlil )
	struct IL
	{
		R_METHOD() static utils::SENameMap* getnamemap()
		{
			::addref( ILNameMap );
			return ILNameMap;
		}
		R_METHOD() static utils::SEContext* getcontext()
		{
			::addref( ILContext );
			return ILContext;
		}
	};
}
