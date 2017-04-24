#include <exl/system/context.hpp>
#include <exl/system/typeinfo/number.hpp>
#include <exl/system/operator/add.hpp>
#include <exl/system/operator/assign.hpp>
#include <exl/system/operator/join.hpp>
#include <exl/system/operator/subtract.hpp>
#include <exl/system/typeinfo/string.hpp>
#include <exl/system/typeinfo/typedef.hpp>
#include <exl/system/value/operator.hpp>
#include <exl/symbol/const.hpp>
#include <exl/value/typedef.hpp>
#include <exl/context.hpp>
#include <exl/format.hpp>

namespace exl
{
	namespace
	{
		Ref< DataBuffer > systemfilename = DataBuffer::create( 8, "<system>" );

		void registertype(
			IContext* context, ITypeInfo* ti, char const* name )
		{
			identifier_t ident( name, name + strlen( name ) );
			StringBuilder defstring;
			defstring << identtostr( ident );
			Ref< IConstValue > expr = Ref< TypedefValue >::create( ti, defstring );
			Ref< ISymbol > symbol = Ref< ConstSymbol >::create(
				DefPos{ TextPos(), systemfilename }, expr );
			Ref< ISymbol > oldsymbol = context->trysetsymbol( ident, symbol );
			ASSERT( !oldsymbol );
		}

		void registeroperator(
			IContext* context, ITypeInfo* ti, char const* name )
		{
			identifier_t ident( name, name + strlen( name ) );
			StringBuilder defstring;
			defstring << identtostr( ident );
			Ref< IConstValue > expr = Ref< OperatorValue >::create(
				ti, defstring );
			Ref< ISymbol > symbol = Ref< ConstSymbol >::create(
				DefPos{ TextPos(), systemfilename }, expr );
			Ref< ISymbol > oldsymbol = context->trysetsymbol( ident, symbol );
			ASSERT( !oldsymbol );
		}

		class SystemContext: public Context
		{
		public:
			SystemContext( name_t const& nameprefix );
			~SystemContext();
		};

		SystemContext::SystemContext( name_t const& nameprefix )
			: Context( nameprefix )
		{
			registertype( this, getnumberti(), "number" );
			registertype( this, getstringti(), "string" );
			registertype( this, gettypedefti(), "type" );
			registeroperator( this, getopaddti(), "add" );
			registeroperator( this, getopassignti(), "assign" );
			registeroperator( this, getopjointi(), "join" );
			registeroperator( this, getopsubtractti(), "subtract" );
		}

		SystemContext::~SystemContext()
		{
		}
	}

	Ref< IContext > const& getsystemcontext()
	{
		char const npstr[] = "system";
		identifier_t npident( npstr, npstr + 6 );
		name_t npfn = { npident };
		static Ref< IContext > systemcontext( new SystemContext( npfn ), 0 );
		return systemcontext;
	}
}
