#include <exl/system/typeinfo/typedef.hpp>
#include <exl/typeinfo/base.hpp>

namespace exl
{
	namespace
	{
		class TypedefTI: public TypeInfo, virtual public ITypeInfo
		{
		public:
			TypedefTI();
			~TypedefTI();
			virtual StringBuilder getdefstring( size_t depth ) override;
		};

		TypedefTI::TypedefTI()
		{
		}

		TypedefTI::~TypedefTI()
		{
		}

		StringBuilder TypedefTI::getdefstring( size_t depth )
		{
			return ".system.@type"_db;
		}
	}

	Ref< ITypeInfo > const& gettypedefti()
	{
		static Ref< ITypeInfo > typedefti( new TypedefTI, 0 );
		return typedefti;
	}
}
