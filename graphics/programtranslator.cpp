#include <graphics/programtranslator.hpp>
#include <graphics/programtranslator-step.hpp>
#include <utils/console.hpp>
#include <cstring>

namespace graphics
{
	void translateprogram( utils::DataBuffer* input, ProgramSource* output )
	{
		programtranslator::TranslatorState ts;
		ts.m_input = input;
		memset( ts.m_attributeused, 0, sizeof( ts.m_attributeused ) );
		memset( ts.m_textureused, 0, sizeof( ts.m_textureused ) );
		while( !ts.iseof() )
		{
			programtranslator::step( &ts );
		}
		utils::Ref< programtranslator::Value > result = ts.pop();
		utils::Ref< programtranslator::StringBuilder > sb = result->getstring();
		size_t len = sb->getlength();
		char* buf = new char[ len + 1 ];
		sb->write( ( uint8_t* )buf, len );
		buf[ len ] = 0;
		LOG( "%s", buf );
		delete[] buf;
	}
}
