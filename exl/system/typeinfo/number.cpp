#include <exl/system/typeinfo/number.hpp>
#include <exl/typeinfo/base.hpp>

namespace exl
{
	namespace
	{
		class NumberTI: public TypeInfo, virtual public ITypeInfo
		{
		public:
			NumberTI();
			~NumberTI();
			virtual StringBuilder getdefstring( size_t depth ) override;
		};

		NumberTI::NumberTI()
		{
		}

		NumberTI::~NumberTI()
		{
		}

		StringBuilder NumberTI::getdefstring( size_t depth )
		{
			return ".system.number"_db;
		}
	}

	Ref< ITypeInfo > const& getnumberti()
	{
		static Ref< ITypeInfo > numberti( new NumberTI, 0 );
		return numberti;
	}
}
