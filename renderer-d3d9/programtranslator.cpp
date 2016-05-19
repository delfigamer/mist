#include <renderer-d3d9/programtranslator.hpp>
#include <renderer-d3d9/programtranslator-shaders.hpp>
#include <renderer-d3d9/programtranslator-step.hpp>
#include <cstring>

namespace graphics
{
	namespace programtranslator
	{
		utils::Ref< utils::DataBuffer > buildstring( StringBuilder* sb )
		{
			size_t len = sb->getlength();
			auto db = utils::DataBuffer::create( len + 1, len + 1, 0 );
			sb->write( db->m_data, db->m_length - 1 );
			db->m_data[ len ] = 0;
			return db;
		}
	}

	void translateprogram( utils::DataBuffer* input, ProgramSource* output )
	{
		programtranslator::TranslatorState ts;
		ts.m_input = input;
		memset( ts.m_attributeused, 0, sizeof( ts.m_attributeused ) );
		ts.m_attributeused[ 0 ] = true;
		memset( ts.m_textureused, 0, sizeof( ts.m_textureused ) );
		while( !ts.iseof() )
		{
			programtranslator::step( &ts );
		}
		utils::Ref< programtranslator::StringBuilder > vsh;
		utils::Ref< programtranslator::StringBuilder > fsh;
		programtranslator::makeshaders( &ts, &vsh, &fsh );
		output->m_vertexsource = programtranslator::buildstring( vsh );
		output->m_fragmentsource = programtranslator::buildstring( fsh );
	}
}
