#include <exl/context.hpp>
#include <exl/func.hpp>

namespace exl
{
	Context::Context()
	{
	}

	Context::~Context()
	{
	}

	Ref< StringBuilder > Context::getdefstring( size_t depth )
	{
		return StringBuilder::construct( "< context >" );
	}
}
