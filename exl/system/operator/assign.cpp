#include <exl/system/operator/assign.hpp>
#include <exl/typeinfo/base.hpp>
#include <exl/value/base.hpp>
#include <exl/il/types.hpp>

namespace exl
{
	namespace
	{
		class InvAssign: public Value, public virtual IInvocation
		{
		private:
			Ref< IValue > m_target;
			Ref< ITypeCast > m_source;

		public:
			InvAssign( IValue* target, ITypeCast* castsource );
			~InvAssign();

			virtual StringBuilder getdefstring( size_t depth ) override;
			virtual Ref< IConstValue > getconstvalue() override;
			virtual void compileread( ILBody* body, ILValue& value ) override;
		};

		class OpAssignTI: public TypeInfo, virtual public ITypeInfo
		{
		public:
			OpAssignTI();
			~OpAssignTI();
			virtual StringBuilder getdefstring( size_t depth ) override;
			virtual Ref< IInvocation > invoke(
				IValue* target,
				std::vector< Ref< IValue > > const& args ) override;
		};

		InvAssign::InvAssign( IValue* target, ITypeCast* castsource )
			: m_target( target )
			, m_source( castsource )
		{
			m_fulltype = FullType{ nullptr, false, false };
		}

		InvAssign::~InvAssign()
		{
		}

		StringBuilder InvAssign::getdefstring( size_t depth )
		{
			StringBuilder defstring;
			defstring
				<< ".system.assign["_db << m_target->getdefstring( depth )
				<< ", "_db << m_source->getdefstring( depth ) << "]"_db;
			return defstring;
		}

		Ref< IConstValue > InvAssign::getconstvalue()
		{
			return nullptr;
		}

		void InvAssign::compileread( ILBody* body, ILValue& value )
		{
			ILValue aval;
			m_source->compileread( body, aval );
			m_target->compilewrite( body, aval );
			value.setnil();
		}

		OpAssignTI::OpAssignTI()
		{
		}

		OpAssignTI::~OpAssignTI()
		{
		}

		StringBuilder OpAssignTI::getdefstring( size_t depth )
		{
			return "type .system.assign"_db;
		}

		Ref< IInvocation > OpAssignTI::invoke(
			IValue* target,
			std::vector< Ref< IValue > > const& args )
		{
			if( args.size() != 2 )
			{
				return nullptr;
			}
			Ref< IValue > left = args[ 0 ];
			FullType leftft = left->getfulltype();
			if( !leftft.writable )
			{
				return nullptr;
			}
			Ref< IValue > right = args[ 1 ];
			FullType rightft = right->getfulltype();
			if( !rightft.readable )
			{
				return nullptr;
			}
			ASSERT( leftft.type );
			Ref< ITypeCast > cast = leftft.type->cast( right );
			if( !cast )
			{
				return nullptr;
			}
			return Ref< InvAssign >::create( left, cast );
		}
	}

	Ref< ITypeInfo > const& getopassignti()
	{
		static Ref< ITypeInfo > opassignti( new OpAssignTI, 0 );
		return opassignti;
	}
}
