#include <exl/system/typeinfo/string.hpp>
#include <exl/typeinfo/base.hpp>

namespace exl
{
	namespace
	{
		class StringTI: public TypeInfo, virtual public ITypeInfo
		{
		public:
			StringTI();
			~StringTI();
			virtual StringBuilder getdefstring( size_t depth ) override;
		};

		StringTI::StringTI()
		{
		}

		StringTI::~StringTI()
		{
		}

		StringBuilder StringTI::getdefstring( size_t depth )
		{
			return ".system.string"_db;
		}
	}

	Ref< ITypeInfo > const& getstringti()
	{
		static Ref< ITypeInfo > stringti( new StringTI, 0 );
		return stringti;
	}
}
