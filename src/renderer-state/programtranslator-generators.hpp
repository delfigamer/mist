#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-base.hpp>
#include <renderer-state/programtranslator-state.hpp>
#include <common/ref.hpp>
#include <common/databuffer.hpp>
#include <common/refobject.hpp>
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
			virtual StringBuilder getstring( StringBuilder* defs ) override;
		};

		struct Value_Texture: Value
		{
			int m_index;

			Value_Texture( TranslatorState* ts, int index );
			// back-end-dependent
			virtual StringBuilder getstring( StringBuilder* defs ) override;
		};

		struct Value_Literal: Value
		{
			std::vector< float > m_elements;

			Value_Literal( int type, float const* vb, float const* ve );
			virtual StringBuilder getstring( StringBuilder* defs ) override;
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

		Value_Literal::Value_Literal( int type, float const* vb, float const* ve )
			: m_elements( vb, ve )
		{
			m_type = type;
		}

		namespace
		{
			Ref< DataBuffer > floattostring( float f )
			{
				char buf[ 32 ];
				int len = snprintf( buf, sizeof( buf ), "%.8e", f );
				return DataBuffer::create( len, buf );
			}
		}

		StringBuilder Value_Literal::getstring( StringBuilder* defs )
		{
			StringBuilder sb =
				typestr[ m_type ] + "("_db + floattostring( m_elements[ 0 ] );
			auto it = ++m_elements.begin();
			auto eit = m_elements.end();
			for( ; it != eit; ++it )
			{
				sb += ","_db + floattostring( *it );
			}
			sb += ")"_db;
			return sb;
		}
	}
}
