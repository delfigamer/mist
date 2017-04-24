#include <exl/value/string.hpp>
#include <exl/system/typeinfo/string.hpp>
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

	uint64_t StringValue::compileread( ILBody* body )
	{
		uint64_t ret = reservereg( body );
		std::unique_ptr< ILStringValue > value( new ILStringValue );
			value->type = ValueType::string;
			value->string = m_content;
		appendtoken( body, makeregassignment( ret, std::move( value ) ) );
		return ret;
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
