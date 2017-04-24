#include <exl/system/operator/subtract.hpp>
#include <exl/system/operator/numberbinary.hpp>

namespace exl
{
	namespace
	{
		class InvSubtract: public InvNumberBinary
		{
		protected:
			virtual double staticvalue( double left, double right ) override;

		public:
			InvSubtract( ITypeCast* left, ITypeCast* right );
			~InvSubtract();

			virtual StringBuilder getdefstring( size_t depth ) override;
		};

		class OpSubtractTI: public OpNumberBinaryTI
		{
		protected:
			virtual Ref< InvBinary > createinvocation(
				ITypeCast* left, ITypeCast* right ) override;

		public:
			OpSubtractTI();
			~OpSubtractTI();

			virtual StringBuilder getdefstring( size_t depth ) override;
		};

		double InvSubtract::staticvalue( double left, double right )
		{
			return left - right;
		}

		InvSubtract::InvSubtract( ITypeCast* left, ITypeCast* right )
			: InvNumberBinary( left, right )
		{
			m_operatortype = TokenType::subtract;
		}

		InvSubtract::~InvSubtract()
		{
		}

		StringBuilder InvSubtract::getdefstring( size_t depth )
		{
			StringBuilder defstring;
			defstring
				<< ".system.subtract["_db << m_left->getdefstring( depth )
				<< ", "_db << m_right->getdefstring( depth ) << "]"_db;
			return defstring;
		}

		Ref< InvBinary > OpSubtractTI::createinvocation(
			ITypeCast* left, ITypeCast* right )
		{
			return Ref< InvSubtract >::create( left, right );
		}

		OpSubtractTI::OpSubtractTI()
		{
		}

		OpSubtractTI::~OpSubtractTI()
		{
		}

		StringBuilder OpSubtractTI::getdefstring( size_t depth )
		{
			return "type .system.subtract"_db;
		}
	}

	Ref< ITypeInfo > const& getopsubtractti()
	{
		static Ref< ITypeInfo > opsubtractti( new OpSubtractTI, 0 );
		return opsubtractti;
	}
}
