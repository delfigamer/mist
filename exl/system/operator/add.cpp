#include <exl/system/operator/add.hpp>
#include <exl/system/operator/numberbinary.hpp>

namespace exl
{
	namespace
	{
		class InvAdd: public InvNumberBinary
		{
		protected:
			virtual double staticvalue( double left, double right ) override;

		public:
			InvAdd( ITypeCast* left, ITypeCast* right );
			~InvAdd();

			virtual StringBuilder getdefstring( size_t depth ) override;
		};

		class OpAddTI: public OpNumberBinaryTI
		{
		protected:
			virtual Ref< InvBinary > createinvocation(
				ITypeCast* left, ITypeCast* right ) override;

		public:
			OpAddTI();
			~OpAddTI();

			virtual StringBuilder getdefstring( size_t depth ) override;
		};

		double InvAdd::staticvalue( double left, double right )
		{
			return left + right;
		}


		InvAdd::InvAdd( ITypeCast* left, ITypeCast* right )
			: InvNumberBinary( left, right )
		{
			m_operatortype = TokenType::add;
		}

		InvAdd::~InvAdd()
		{
		}

		StringBuilder InvAdd::getdefstring( size_t depth )
		{
			StringBuilder defstring;
			defstring
				<< ".system.add["_db << m_left->getdefstring( depth )
				<< ", "_db << m_right->getdefstring( depth ) << "]"_db;
			return defstring;
		}

		Ref< InvBinary > OpAddTI::createinvocation(
			ITypeCast* left, ITypeCast* right )
		{
			return Ref< InvAdd >::create( left, right );
		}

		OpAddTI::OpAddTI()
		{
		}

		OpAddTI::~OpAddTI()
		{
		}

		StringBuilder OpAddTI::getdefstring( size_t depth )
		{
			return "type .system.add"_db;
		}
	}

	Ref< ITypeInfo > const& getopaddti()
	{
		static Ref< ITypeInfo > opaddti( new OpAddTI, 0 );
		return opaddti;
	}
}
