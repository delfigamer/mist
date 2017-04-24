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

	uint64_t FunctionValue::compileread( ILBody* body )
	{
		ILBody funcbody;
		std::vector< uint64_t > arguments;
		{
			initilbody( &funcbody );
			funcbody.depth = body->depth + 1;
			double_for(
				m_argdefs, m_argsymbols,
				[ & ]( ArgDef& argdef, ISymbol* symbol )
				{
					uint64_t reg;
					if( argdef.fulltype.readable )
					{
						reg = reservereg( &funcbody );
						arguments.push_back( reg );
					}
					else
					{
						reg = reservereg( &funcbody );
						appendtoken( &funcbody, makeregassignment( reg, 0 ) );
					}
					symbol->setregister( reg );
					std::unique_ptr< ILSymbolNote > note( new ILSymbolNote );
						note->type = TokenType::symbolnote;
						note->reg = reg;
						note->symboltype = SymbolType::symargument;
						if( argdef.named )
						{
							note->fullname = appendident(
								m_subcontext->getnameprefix(), argdef.name );
						}
						note->typeinfo =
							argdef.fulltype.type->getdefstring( 0 ).combine();
						note->defpos = argdef.defpos;
					funcbody.notes.push_back( std::move( note ) );
				} );
			m_body->compilereserve( &funcbody );
			m_body->compileemit( &funcbody );
			funcbody.currentblock->branches.push_back(
				ILBranch{ nullptr, funcbody.exitblock } );
		}
		uint64_t ret = reservereg( body );
		std::unique_ptr< ILFunctionValue > value( new ILFunctionValue );
			value->type = ValueType::function;
			value->args = std::move( arguments );
			value->body = std::move( funcbody );
		appendtoken( body, makeregassignment( ret, std::move( value ) ) );
		return ret;
	}
}
