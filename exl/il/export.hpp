#pragma once

#include <exl/il/types.hpp>
#include <utils/sexpr.hpp>
#include <common/ref.hpp>
#include <common.hpp>

namespace exl
{
	extern Ref< utils::SENameMap > ILNameMap;
	extern Ref< utils::SEContext > ILContext;

	utils::SExpr exportmodule( ILModule const& module );

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
