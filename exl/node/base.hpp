#pragma once

#include <exl/object.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common/stringbuilder.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <cstdarg>

namespace exl
{
	class Node: public Object, public virtual INode
	{
	protected:
		TextRange m_textrange;

	public:
		Node();
		Node( utils::SExpr const& s );
		~Node();

		virtual TextRange gettextrange() override;
	};
}
