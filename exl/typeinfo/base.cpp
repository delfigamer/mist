#include <exl/typeinfo/base.hpp>
#include <exl/value/identitycast.hpp>
#include <exl/func.hpp>

namespace exl
{
	TypeInfo::TypeInfo()
	{
	}

	TypeInfo::~TypeInfo()
	{
	}

	Ref< ITypeCast > TypeInfo::cast( IValue* source )
	{
		FullType otherft = source->getfulltype();
		if( otherft.type == this )
		{
			return Ref< IdentityCast >::create( source );
		}
		else
		{
			return nullptr;
		}
	}

	Ref< IInvocation > TypeInfo::invoke(
		IValue* target,
		std::vector< Ref< IValue > > const& args )
	{
		return nullptr;
	}
}
