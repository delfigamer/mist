#pragma once

#include <exl/object.hpp>
#include <exl/types.hpp>
#include <common/stringbuilder.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	class Context: public Object, public virtual IContext
	{
	public:
		Context();
		~Context();

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;

		// R_METHOD() static Context* create()
		// {
			// return new Context;
		// }
		// R_METHOD() DataBuffer* getdefstring()
		// {
			// Ref< StringBuilder > sb = getdefstring( 0 );
			// size_t len = sb->getlength();
			// Ref< DataBuffer > ret = DataBuffer::create( len, len, nullptr );
			// sb->write( ret->m_data, len );
			// return ret.detach();
		// }
	};
}
