#pragma once

#include <graphics/programtranslator.hpp>
#include <graphics/programtranslator-base.hpp>
#include <graphics/programtranslator-state.hpp>
#include <utils/ref.hpp>
#include <utils/databuffer.hpp>
#include <utils/refobject.hpp>
#include <cstdio>
#include <cstdarg>

namespace graphics
{
	namespace programtranslator
	{
		struct Value_Attribute: Value
		{
			int m_index;

			Value_Attribute( TranslatorState* ts, int index );
			virtual utils::Ref< StringBuilder > getstring() override;
		};

		struct Value_Texture: Value
		{
			int m_index;

			Value_Texture( TranslatorState* ts, int index );
			virtual utils::Ref< StringBuilder > getstring() override;
		};

		struct Value_Literal: Value
		{
			std::vector< float > m_elements;

			Value_Literal(
				int type, float const* vb, float const* ve );
			virtual utils::Ref< StringBuilder > getstring() override;
		};

		Value_Attribute::Value_Attribute( TranslatorState* ts, int index )
			: m_index( index )
		{
			m_type = valuetype::float4;
			ts->m_attributeused[ index ] = true;
		}

		utils::Ref< StringBuilder > Value_Attribute::getstring()
		{
			static utils::String const attrstr[] = {
				"attrs.a0",
				"attrs.a1",
				"attrs.a2",
				"attrs.a3",
				"attrs.a4",
				"attrs.a5",
				"attrs.a6",
				"attrs.a7",
			};
			return StringBuilder::construct( attrstr[ m_index ] );
		}

		Value_Texture::Value_Texture( TranslatorState* ts, int index )
			: m_index( index )
		{
			m_type = valuetype::sampler2D;
			ts->m_textureused[ index ] = true;
		}

		utils::Ref< StringBuilder > Value_Texture::getstring()
		{
			static utils::String const texstr[] = {
				"texture_0",
				"texture_1",
				"texture_2",
				"texture_3",
				"texture_4",
				"texture_5",
				"texture_6",
				"texture_7",
			};
			return StringBuilder::construct( texstr[ m_index ] );
		}

		Value_Literal::Value_Literal(
			int type, float const* vb, float const* ve )
			: m_elements( vb, ve )
		{
			m_type = type;
		}

		utils::String floattostring( float f )
		{
			char buf[ 32 ];
			snprintf( buf, sizeof( buf ), "%.8e", f );
			return utils::String( buf );
		}

		utils::Ref< StringBuilder > Value_Literal::getstring()
		{
			static utils::String const typestr[] = {
				"float",
				"float2",
				"float3",
				"float4",
				"float2x2",
				"float3x3",
				"float4x4",
			};
			auto sb = StringBuilder::construct(
				typestr[ m_type ], "(", floattostring( m_elements[ 0 ] ) );
			auto it = ++m_elements.begin();
			auto eit = m_elements.end();
			for( ; it != eit; ++it )
			{
				sb = StringBuilder::construct(
					sb, ",", floattostring( *it ) );
			}
			return StringBuilder::construct(
					sb, ")" );
		}
	}
}
