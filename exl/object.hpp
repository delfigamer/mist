#pragma once

#include <exl/types.hpp>
#include <common/refobject.hpp>
#include <common.hpp>

namespace exl
{
	class Object: public RefObject, public virtual IObject
	{
	public:
		Object();
		~Object();
		Object( Object const& other ) = delete;
		Object& operator=( Object const& other ) = delete;
		virtual ptrdiff_t AddRef() override;
		virtual ptrdiff_t Release() override;
	};
}
