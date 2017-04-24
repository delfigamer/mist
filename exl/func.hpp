#pragma once

#include <exl/types.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <vector>
#include <tuple>

namespace exl
{
	identifier_t anonymousident( TextRange range );
	identifier_t anonymousident( DefPos pos );
	name_t appendident( name_t const& prefix, identifier_t const& ident );
	Ref< ITypeInfo > gettypevalue( IExpression* expr );
	void setsymbol(
		IContext* context, identifier_t const& name, ISymbol* symbol );
	void initilbody( ILBody* body );
	uint64_t reservereg( ILBody* body );
	void releasereg( ILBody* body, uint64_t reg );
	void createblock( ILBody* body );
	void appendtoken( ILBody* body, std::unique_ptr< ILToken >&& token );
	std::unique_ptr< ILValue > makeregvalue( uint64_t reg );
	std::unique_ptr< ILToken > makeregassignment(
		uint64_t target, uint64_t source );
	std::unique_ptr< ILToken > makeregassignment(
		uint64_t target, std::unique_ptr< ILValue >&& source );

	template< typename TS, typename TF >
	auto apply(
		TS&& source, TF const& func )
		-> std::vector< decltype( func( *std::begin( source ) ) ) >
	{
		std::vector< decltype( func( *std::begin( source ) ) ) > ret;
		ret.reserve( source.size() );
		for( auto& sourceit: source )
		{
			ret.push_back( func( sourceit ) );
		}
		return ret;
	}

	template< typename TS, typename TT, typename TC >
	auto apply(
		TS&& source, TT( TC::*method )() )
		-> std::vector< decltype( ( ( *std::begin( source ) )->*method )() ) >
	{
		return apply(
			source,
			[ method ]( decltype( *std::begin( source ) ) sit ) -> TT
			{
				return ( sit->*method )();
			} );
	}

	template< typename TA, typename TB, typename TF >
	void double_for(
		TA&& a, TB&& b, TF func )
	{
		auto ait = std::begin( a );
		auto aeit = std::end( a );
		auto bit = std::begin( b );
		while( ait != aeit )
		{
			func( *ait, *bit );
			++ait;
			++bit;
		}
		ASSERT( bit == std::end( b ) );
	}
}
