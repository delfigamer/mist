#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-shaders.hpp>
#include <renderer-state/programtranslator-step.hpp>
#include <cstring>

namespace graphics
{
	void translateprogram(
		DataBuffer* source, Ref< DataBuffer >* vsrc, Ref< DataBuffer >* fsrc )
	{
		programtranslator::TranslatorState ts;
		ts.m_input = source;
		memset( ts.m_attributeused, 0, sizeof( ts.m_attributeused ) );
		ts.m_attributeused[ 0 ] = true;
		memset( ts.m_textureused, 0, sizeof( ts.m_textureused ) );
		while( !ts.iseof() )
		{
			programtranslator::step( &ts );
		}
		StringBuilder vsh;
		StringBuilder fsh;
		programtranslator::makeshaders( &ts, &vsh, &fsh );
		*vsrc = vsh.combine();
		*fsrc = fsh.combine();
	}
}
