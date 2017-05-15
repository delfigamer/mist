#include <exl/value/string.hpp>
#include <exl/system/typeinfo/string.hpp>
#include <exl/il/types.hpp>
#include <exl/func.hpp>
#include <exl/format.hpp>

namespace exl
{
	StringValue::StringValue( DataBuffer* content )
		: m_content( content )
	{
		m_fulltype.type = getstringti();
	}

	StringValue::~StringValue()
	{
	}

	StringBuilder StringValue::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "\""_db << bytestostr( m_content ) << "\""_db;
		return defstring;
	}

	void StringValue::compileread( ILBody* body, ILValue& value )
	{
		if( m_content )
		{
			if( m_content->m_length <= 20 )
			{
				value.setstring( m_content->m_length, m_content->m_data );
			}
			else
			{
				ILStringConst* sconst = body->appendconstant< ILStringConst >();
				sconst->type = ConstType::string;
				sconst->string = m_content;
				value.setconstant( sconst );
			}
		}
		else
		{
			value.setstring();
		}
	}

	Ref< IStringValue > StringValue::getstringvalue()
	{
		return this;
	}

	Ref< DataBuffer > StringValue::getstringcontent()
	{
		return m_content;
	}
}
