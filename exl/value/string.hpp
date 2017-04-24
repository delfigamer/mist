#pragma once

#include <exl/value/literal.hpp>
#include <common.hpp>

namespace exl
{
	class StringValue: public LiteralValue, public virtual IStringValue
	{
	private:
		Ref< DataBuffer > m_content;

	public:
		StringValue( DataBuffer* content );
		~StringValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual uint64_t compileread( ILBody* body ) override;
		virtual Ref< IStringValue > getstringvalue() override;
		virtual Ref< DataBuffer > getstringcontent() override;
	};
}
