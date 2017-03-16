#pragma once

#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	R_CLASS( name = exlnode )
	class NodeContainer: public RefObject
	{
	private:
		Ref< IBlock > m_node;
		Ref< DataBuffer > m_defstring;

	public:
		NodeContainer( Ref< IBlock > const& node );
		NodeContainer( Ref< IBlock >&& node );
		~NodeContainer();
		NodeContainer( NodeContainer const& other ) = delete;
		NodeContainer& operator=( NodeContainer const& other ) = delete;

		R_METHOD() static NodeContainer* create( utils::SExpr const* s );
		R_METHOD( addref ) DataBuffer* getdefstring();
	};
}
