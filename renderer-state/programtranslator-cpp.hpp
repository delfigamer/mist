#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-shaders.hpp>
#include <renderer-state/programtranslator-step.hpp>
#include <cstring>

namespace graphics
{
	namespace programtranslator
	{
		Ref< DataBuffer > buildstring( StringBuilder* sb )
		{
			size_t len = sb->getlength();
			auto db = DataBuffer::create( len + 1, len + 1, 0 );
			sb->write( db->m_data, db->m_length - 1 );
			db->m_data[ len ] = 0;
			return db;
		}
	}

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
		Ref< programtranslator::StringBuilder > vsh;
		Ref< programtranslator::StringBuilder > fsh;
		programtranslator::makeshaders( &ts, &vsh, &fsh );
		*vsrc = programtranslator::buildstring( vsh );
		*fsrc = programtranslator::buildstring( fsh );
	}
}
