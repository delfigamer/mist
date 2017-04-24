#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-base.hpp>
#include <renderer-state/programtranslator-state.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <tuple>
#include <initializer_list>
#include <vector>
#include <map>

namespace graphics
{
	namespace programtranslator
	{
		struct Value_Swizzle: Value
		{
			Ref< Value > m_arg;
			int m_parts[ 4 ];

			Value_Swizzle(
				TranslatorState* ts,
				Ref< Value > const& arg, int const* parts );
			virtual StringBuilder getstring( StringBuilder* defs ) override;
		};

		Value_Swizzle::Value_Swizzle(
			TranslatorState* ts,
			Ref< Value > const& arg, int const* parts )
			: m_arg( arg )
		{
			int maxpart;
			switch( arg->m_type )
			{
				case valuetype::float2:
					maxpart = 1; break;
				case valuetype::float3:
					maxpart = 2; break;
				case valuetype::float4:
					maxpart = 3; break;
				default:
					ts->error( "non-vector argument" );
					break;
			}
			int rlen = 4;
			for( int i = 0; i < 4; ++i )
			{
				if( parts[ i ] > maxpart )
				{
					ts->error( "invalid swizzle" );
				}
				m_parts[ i ] = parts[ i ];
				if( parts[ i ] < 0 )
				{
					rlen = i;
					break;
				}
			}
			ASSERT( rlen > 0 && rlen <= 4 );
			m_type = valuetype::float1 + rlen - 1;
			arg->m_usecount += 1;
		}

		static Ref< DataBuffer > const swizzle_indices[] = {
			"x"_db,
			"y"_db,
			"z"_db,
			"w"_db };

		StringBuilder Value_Swizzle::getstring( StringBuilder* defs )
		{
			switch( m_type )
			{
				case valuetype::float1:
					return StringBuilder()
						<< "("_db << m_arg->getstring( defs ) << "."_db
						<< swizzle_indices[ m_parts[ 0 ] ] << ")"_db;
				case valuetype::float2:
					return StringBuilder()
						<< "("_db << m_arg->getstring( defs ) << "."_db
						<< swizzle_indices[ m_parts[ 0 ] ]
						<< swizzle_indices[ m_parts[ 1 ] ] << ")"_db;
				case valuetype::float3:
					return StringBuilder()
						<< "("_db << m_arg->getstring( defs ) << "."_db
						<< swizzle_indices[ m_parts[ 0 ] ]
						<< swizzle_indices[ m_parts[ 1 ] ]
						<< swizzle_indices[ m_parts[ 2 ] ] << ")"_db;
				case valuetype::float4:
					return StringBuilder()
						<< "("_db << m_arg->getstring( defs ) << "."_db
						<< swizzle_indices[ m_parts[ 0 ] ]
						<< swizzle_indices[ m_parts[ 1 ] ]
						<< swizzle_indices[ m_parts[ 2 ] ]
						<< swizzle_indices[ m_parts[ 3 ] ] << ")"_db;
			}
			ASSERT( false );
			return nullptr;
		}
	}
}
