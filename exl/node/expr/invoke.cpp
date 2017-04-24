#include <exl/node/expr/invoke.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/error.hpp>
#include <exl/format.hpp>
#include <map>
#include <cstring>

namespace exl
{
	InvokeExpr::InvokeExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_invoke );
		ASSERT( s.list.size() == 3 );
		ASSERT( s[ 3 ].head == 0 );
		m_simplename = false;
		if( s[ 2 ].head == NodeHead::expr_name )
		{
			utils::SExpr const& nameexpr = s[ 2 ];
			if( nameexpr.list.size() == 2 && nameexpr[ 2 ].head == NodeHead::name )
			{
				utils::SExpr const& name = nameexpr[ 2 ];
				if( name.list.size() == 1 )
				{
					m_simplename = true;
					m_name = constructidentifier( name[ 1 ] );
				}
			}
		}
		m_target = constructexpression( s[ 2 ] );
		m_args = apply( s[ 3 ].list, constructexpression );
	}

	InvokeExpr::~InvokeExpr()
	{
	}

	namespace
	{
		identifier_t ident( char const* str )
		{
			return identifier_t( str, str + strlen( str ) );
		}

		std::map< identifier_t, Ref< DataBuffer > > unarymap = {
			{ ident( "identity" ), "+ "_db },
			{ ident( "negate" ), "- "_db },
		};

		std::map< identifier_t, Ref< DataBuffer > > binarymap = {
			{ ident( "add" ), " + "_db },
			{ ident( "addto" ), " += "_db },
			{ ident( "assign" ), " = "_db },
			{ ident( "join" ), " .. "_db },
			{ ident( "multiply" ), " * "_db },
			{ ident( "subtract" ), " - "_db },
			{ ident( "subtractfrom" ), " -= "_db },
		};
	}

	StringBuilder InvokeExpr::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		if( m_simplename )
		{
			if( m_args.size() == 1 )
			{
				auto it = unarymap.find( m_name );
				if( it != unarymap.end() )
				{
					StringBuilder defstring;
					defstring
						<< it->second << m_args[ 0 ]->getdefstring( depth );
					return defstring;
				}
			}
			else if( m_args.size() == 2 )
			{
				auto it = binarymap.find( m_name );
				if( it != binarymap.end() )
				{
					StringBuilder defstring;
					defstring
						<< m_args[ 0 ]->getdefstring( depth )
						<< it->second << m_args[ 1 ]->getdefstring( depth );
					return defstring;
				}
			}
			defstring << identtostr( m_name );
		}
		else
		{
			defstring << m_target->getdefstring( depth );
		}
		defstring
			<< "["_db << sbconcat( m_args, depth, ", "_db ) << "]"_db;
		return defstring;
	}

	void InvokeExpr::build( IContext* context )
	{
		m_target->build( context );
		for( auto& arg: m_args )
		{
			arg->build( context );
		}
		Ref< IValue > targetvalue = m_target->getvalue();
		std::vector< Ref< IValue > > argvalues = apply(
			m_args, &IExpression::getvalue );
		FullType targetft = targetvalue->getfulltype();
		ASSERT( targetft.type );
		m_value = targetft.type->invoke( targetvalue, std::move( argvalues ) );
		if( !m_value )
		{
			invokeerror( gettextrange(), targetft.type, m_args );
		}
	}
}
