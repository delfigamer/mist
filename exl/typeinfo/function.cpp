#include <exl/typeinfo/function.hpp>
#include <exl/value/base.hpp>
#include <exl/func.hpp>
#include <exl/format.hpp>
#include <vector>
#include <stack>

namespace exl
{
	namespace
	{
		class FunctionCall: public Value, public virtual IInvocation
		{
		private:
			Ref< FunctionTI > m_functiontype;
			Ref< IValue > m_target;
			std::vector< Ref< ITypeCast > > m_args;

		public:
			FunctionCall(
				FunctionTI* functiontype, IValue* target,
				std::vector< Ref< ITypeCast > >&& args, ITypeInfo* rettype );
			~FunctionCall();

			virtual StringBuilder getdefstring( size_t depth ) override;
			virtual Ref< IConstValue > getconstvalue() override;
			virtual void compileread( ILBody* body, ILValue& value ) override;
		};

		FunctionCall::FunctionCall(
				FunctionTI* functiontype, IValue* target,
				std::vector< Ref< ITypeCast > >&& args, ITypeInfo* rettype )
			: m_functiontype( functiontype )
			, m_target( target )
			, m_args( std::move( args ) )
		{
			if( rettype )
			{
				m_fulltype = FullType{ rettype, true, false };
			}
			else
			{
				m_fulltype = FullType{ nullptr, false, false };
			}
		}

		FunctionCall::~FunctionCall()
		{
		}

		StringBuilder FunctionCall::getdefstring( size_t depth )
		{
			StringBuilder defstring;
			defstring
				<< m_target->getdefstring( depth ) << "["_db
				<< sbconcat( m_args, depth, ", "_db ) << "]"_db;
			return defstring;
		}

		Ref< IConstValue > FunctionCall::getconstvalue()
		{
			return nullptr;
		}

		struct outarg_t
		{
			ILValue val;
			IValue* arg;
		};

		void FunctionCall::compileread( ILBody* body, ILValue& result )
		{
			ILValue target;
			m_target->compileread( body, target );
			std::vector< ILValue > inputs;
			inputs.push_back( target );
			std::vector< ILValue > outputs;
			double_for( m_functiontype->argdefs(), m_args,
				[ & ]( FullType const& argft, IValue* arg )
				{
					if( argft.readable )
					{
						ILValue val;
						arg->compileread( body, val );
						inputs.push_back( val );
					}
				} );
			std::stack< outarg_t > outargs;
			if( m_fulltype.readable )
			{
				body->newtemp( result );
				outputs.push_back( result );
			}
			double_for( m_functiontype->argdefs(), m_args,
				[ & ]( FullType const& argft, IValue* arg )
				{
					if( argft.writable )
					{
						ILValue val;
						body->newtemp( val );
						outputs.push_back( val );
						outargs.push( outarg_t{ val, arg } );
					}
				} );
			ILToken* op = body->appendtoken();
			op->type = TokenType::invoke;
			op->inputs = std::move( inputs );
			op->outputs = std::move( outputs );
			while( !outargs.empty() )
			{
				outarg_t& oa = outargs.top();
				oa.arg->compilewrite( body, oa.val );
				outargs.pop();
			}
		}
	}

	FunctionTI::FunctionTI(
		std::vector< ArgDef > const& argdeflist, ITypeInfo* rettypeinfo )
		: m_rettypeinfo( rettypeinfo )
	{
		m_argdefs = apply(
			argdeflist,
			[]( ArgDef const& argdef ) -> FullType
			{
				return argdef.fulltype;
			} );
	}

	FunctionTI::~FunctionTI()
	{
	}

	namespace
	{
		StringBuilder fulltypetostr( FullType const& ft, size_t depth )
		{
			StringBuilder ret;
			if( ft.readable )
			{
				if( ft.writable )
				{
					ret << "inout: "_db;
				}
				else
				{
					ret << ": "_db;
				}
			}
			else
			{
				if( ft.writable )
				{
					ret << "out: "_db;
				}
				else
				{
					ret << "void"_db;
					return ret;
				}
			}
			ret << ft.type->getdefstring( depth );
			return ret;
		}
	}

	StringBuilder FunctionTI::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring
			<< "type function["_db
			<< sbconcat( m_argdefs, depth, ", "_db, fulltypetostr )
			<< "]"_db;
		return defstring;
	}

	Ref< IInvocation > FunctionTI::invoke(
		IValue* target,
		std::vector< Ref< IValue > > const& args )
	{
		if( args.size() != m_argdefs.size() )
		{
			return nullptr;
		}
		std::vector< Ref< ITypeCast > > castargs;
		castargs.reserve( m_argdefs.size() );
		auto argit = args.begin();
		auto argdefit = m_argdefs.begin();
		auto argdefeit = m_argdefs.end();
		for( ; argdefit != argdefeit; ++argit, ++argdefit )
		{
			Ref< IValue > arg = *argit;
			FullType const& argdef = *argdefit;
			FullType argft = arg->getfulltype();
			if( argdef.readable and !argft.readable )
			{
				return nullptr;
			}
			if( argdef.writable and !argft.writable )
			{
				return nullptr;
			}
			Ref< ITypeCast > cast = argdef.type->cast( arg );
			if( !cast )
			{
				return nullptr;
			}
			castargs.push_back( std::move( cast ) );
		}
		return Ref< FunctionCall >::create(
			this, target, std::move( castargs ), m_rettypeinfo );
	}

	std::vector< FullType >& FunctionTI::argdefs()
	{
		return m_argdefs;
	}
}
