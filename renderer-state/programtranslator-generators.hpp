#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-base.hpp>
#include <renderer-state/programtranslator-state.hpp>
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
			// back-end-dependent
			virtual utils::Ref< StringBuilder > getstring(
				utils::Ref< StringBuilder >* defs ) override;
		};

		struct Value_Texture: Value
		{
			int m_index;

			Value_Texture( TranslatorState* ts, int index );
			// back-end-dependent
			virtual utils::Ref< StringBuilder > getstring(
				utils::Ref< StringBuilder >* defs ) override;
		};

		struct Value_Literal: Value
		{
			std::vector< float > m_elements;

			Value_Literal(
				int type, float const* vb, float const* ve );
			virtual utils::Ref< StringBuilder > getstring(
				utils::Ref< StringBuilder >* defs ) override;
		};

		Value_Attribute::Value_Attribute( TranslatorState* ts, int index )
			: m_index( index )
		{
			m_type = valuetype::float4;
			ts->m_attributeused[ index ] = true;
		}

		Value_Texture::Value_Texture( TranslatorState* ts, int index )
			: m_index( index )
		{
			m_type = valuetype::sampler2D;
			ts->m_textureused[ index ] = true;
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

		utils::Ref< StringBuilder > Value_Literal::getstring(
			utils::Ref< StringBuilder >* defs )
		{
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
