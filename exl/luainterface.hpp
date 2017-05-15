#pragma once

#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class ModuleContainer;

	R_CLASS( name = exlcontext )
	class ContextContainer: public RefObject
	{
	public:
		Ref< IContext > m_context;

	public:
		ContextContainer( char const* nameprefix, ContextContainer* parent );
		ContextContainer( Ref< IContext > const& context );
		ContextContainer( Ref< IContext >&& context );
		~ContextContainer();
		ContextContainer( ContextContainer const& other ) = delete;
		ContextContainer& operator=( ContextContainer const& other ) = delete;

		R_METHOD() static ContextContainer* create(
			char const* nameprefix, ContextContainer* parent )
		{
			return new ContextContainer( nameprefix, parent );
		}
		R_METHOD() DataBuffer* getdefstring();
		R_METHOD() static ContextContainer* getsystemcontext();
	};

	R_CLASS( name = exlnode )
	class NodeContainer: public RefObject
	{
	public:
		Ref< IFileNode > m_node;

	public:
		NodeContainer( utils::SExpr const* s );
		~NodeContainer();
		NodeContainer( NodeContainer const& other ) = delete;
		NodeContainer& operator=( NodeContainer const& other ) = delete;

		R_METHOD() static NodeContainer* create( utils::SExpr const* s )
		{
			return new NodeContainer( s );
		}
		R_METHOD() DataBuffer* getdefstring();
		R_METHOD() void build( ContextContainer* context );
		R_METHOD() ModuleContainer* compile();
	};

	R_CLASS( name = exlmodule )
	class ModuleContainer: public RefObject
	{
	public:
		Ref< IFileNode > m_node;
		ILModule& m_module;

	public:
		ModuleContainer( IFileNode* node, ILModule& module );
		~ModuleContainer();
		ModuleContainer( ModuleContainer const& other ) = delete;
		ModuleContainer& operator=( ModuleContainer const& other ) = delete;

		R_METHOD( gc = methodlist.sexpr_delete, name = export )
		utils::SExpr* sexport();
	};
}
