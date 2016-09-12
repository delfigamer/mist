#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-base.hpp>
#include <renderer-state/programtranslator-state.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
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
			utils::Ref< Value > m_arg;
			int m_parts[ 4 ];

			Value_Swizzle(
				TranslatorState* ts,
				utils::Ref< Value > const& arg, int const* parts );
			virtual utils::Ref< StringBuilder > getstring(
				utils::Ref< StringBuilder >* defs ) override;
		};

		Value_Swizzle::Value_Swizzle(
			TranslatorState* ts,
			utils::Ref< Value > const& arg, int const* parts )
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

		static utils::String const swizzle_indices[] = { "x", "y", "z", "w" };

		utils::Ref< StringBuilder > Value_Swizzle::getstring(
			utils::Ref< StringBuilder >* defs )
		{
			switch( m_type )
			{
				case valuetype::float1:
					return StringBuilder::construct(
						"(", m_arg->getstring( defs ), ".",
						swizzle_indices[ m_parts[ 0 ] ], ")" );
				case valuetype::float2:
					return StringBuilder::construct(
						"(", m_arg->getstring( defs ), ".",
						swizzle_indices[ m_parts[ 0 ] ],
						swizzle_indices[ m_parts[ 1 ] ], ")" );
				case valuetype::float3:
					return StringBuilder::construct(
						"(", m_arg->getstring( defs ), ".",
						swizzle_indices[ m_parts[ 0 ] ],
						swizzle_indices[ m_parts[ 1 ] ],
						swizzle_indices[ m_parts[ 2 ] ], ")" );
				case valuetype::float4:
					return StringBuilder::construct(
						"(", m_arg->getstring( defs ), ".",
						swizzle_indices[ m_parts[ 0 ] ],
						swizzle_indices[ m_parts[ 1 ] ],
						swizzle_indices[ m_parts[ 2 ] ],
						swizzle_indices[ m_parts[ 3 ] ], ")" );
			}
			ASSERT( false );
			return StringBuilder::construct();
		}
	}
}
