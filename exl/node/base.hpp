#pragma once

#include <exl/object.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common/stringbuilder.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	class Node: public Object, public virtual INode
	{
	protected:
		int m_sposrow;
		int m_sposcol;
		int m_eposrow;
		int m_eposcol;
		Ref< DataBuffer > m_filename;
		Ref< StringBuilder > m_defstring;

	public:
		Node();
		Node( utils::SExpr const& s );
		~Node();

		virtual void gettextrange(
			int* sr, int* sc, int* er, int* ec,
			Ref< DataBuffer >* filename ) override;
	};
}
