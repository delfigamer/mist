#include <exl/value/function.hpp>
#include <exl/context.hpp>
#include <exl/typeinfo/function.hpp>
#include <exl/func.hpp>
#include <exl/format.hpp>

namespace exl
{
	FunctionValue::FunctionValue(
		std::vector< ArgDef > const& argdefs, ITypeInfo* rettype,
		IBlock* body, IContext* context )
		: m_argdefs( argdefs )
		, m_rettype( rettype )
		, m_body( body )
	{
		name_t nameprefix = appendident(
			context->getnameprefix(),
			anonymousident( body->gettextrange() ) );
		m_subcontext = Ref< Context >::create( nameprefix, context );
		m_argsymbols = apply(
			argdefs,
			[ this ]( ArgDef const& argdef ) -> Ref< ISymbol >
			{
				Ref< ISymbol > symbol = Ref< LocalSymbol >::create(
					argdef.defpos, argdef.fulltype.type );
				if( argdef.named )
				{
					setsymbol( m_subcontext, argdef.name, symbol );
				}
				return symbol;
			} );
		m_body->build( m_subcontext );
		m_fulltype.type = Ref< FunctionTI >::create( argdefs, rettype );
	}

	FunctionValue::~FunctionValue()
	{
	}

	namespace
	{
		StringBuilder argdeftostr( ArgDef const& argdef, size_t depth )
		{
			StringBuilder ret;
			if( argdef.fulltype.readable )
			{
				if( argdef.fulltype.writable )
				{
					ret << "inout"_db;
				}
				else
				{
				}
			}
			else
			{
				if( argdef.fulltype.writable )
				{
					ret << "out"_db;
				}
				else
				{
					if( argdef.named )
					{
						ret << "void "_db << identtostr( argdef.name );
					}
					else
					{
						ret << "void"_db;
					}
					if( argdef.initvalue )
					{
						ret
							<< " = "_db << argdef.initvalue->getdefstring( depth );
					}
					return ret;
				}
			}
			if( argdef.named )
			{
				ret << ( ret ? " "_db : nullptr ) << identtostr( argdef.name );
			}
			if( argdef.initvalue )
			{
				ret
					<< ( ret ? " = "_db : "= "_db )
					<< argdef.initvalue->getdefstring( depth );
			}
			ret << ": "_db << argdef.fulltype.type->getdefstring( depth );
			return ret;
		}
	}

	StringBuilder FunctionValue::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring
			<< "function["_db
			<< sbconcat( m_argdefs, depth, ", "_db, argdeftostr ) << "]"_db;
		if( m_rettype )
		{
			defstring << ": "_db << m_rettype->getdefstring( depth );
		}
		defstring << m_body->getdefstring( depth + 1 )
			<< "\n"_db << lineprefix( depth ) << "end"_db;
		defstring << "\n"_db << lineprefix( depth )
			<< m_subcontext->getdefstring( depth )
			<< "\n"_db << lineprefix( depth );
		return defstring;
	}

	void FunctionValue::compileread( ILBody* body, ILValue& result )
	{
		ILFunctionConst* fconst = body->appendconstant< ILFunctionConst >();
		{
			fconst->type = ConstType::function;
			fconst->body.reset( body );
			ILStringConst* fnconst =
				fconst->body.appendconstant< ILStringConst >();
			fnconst->type = ConstType::string;
			fnconst->string = m_body->gettextrange().filename;
			double_for(
				m_argdefs, m_argsymbols,
				[ & ]( ArgDef& argdef, ISymbol* symbol )
				{
					ILSymbolDef* symboldef = fconst->body.appendsymbol();
					if( argdef.named )
					{
						symboldef->fullname = appendident(
							m_subcontext->getnameprefix(), argdef.name );
					}
					else
					{
						symboldef->fullname = appendident(
							m_subcontext->getnameprefix(),
							anonymousident( argdef.defpos ) );
					}
					symboldef->typeinfo =
						argdef.fulltype.type->getdefstring( 0 ).combine();
					symboldef->defpos = argdef.defpos;
					ILValue ilvalue;
					ilvalue.setvariable( symboldef->depth, symboldef->index );
					if( argdef.fulltype.readable )
					{
						fconst->args.push_back( symboldef->index );
					}
					else
					{
						fconst->body.appendassignment( ilvalue );
					}
					if( argdef.fulltype.writable )
					{
						fconst->results.push_back( ilvalue );
					}
					symbol->setvalue( ilvalue );
				} );
			m_body->compile( &fconst->body );
			ILBranch* branch = fconst->body.currentblock->appendbranch();
			branch->condition.setboolean( true );
			branch->target = fconst->body.exitblock;
		}
		result.setconstant( fconst );
	}
}
