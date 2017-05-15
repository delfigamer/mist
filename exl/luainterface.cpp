#include <exl/luainterface.hpp>
#include <exl/system/context.hpp>
#include <exl/il/export.hpp>
#include <exl/construct.hpp>
#include <exl/context.hpp>
#include <exl/func.hpp>
#include <cstring>

namespace exl
{
	ContextContainer::ContextContainer(
		char const* nameprefix, ContextContainer* parent )
	{
		name_t name;
		if( nameprefix )
		{
			while( true )
			{
				char const* next = nameprefix + strcspn( nameprefix, "." );
				name.emplace_back( nameprefix, next );
				if( next[ 0 ] == '.' )
				{
					nameprefix = next + 1;
				}
				else
				{
					break;
				}
			}
		}
		m_context = Ref< Context >::create(
			name, parent ? parent->m_context : nullptr );
	}

	ContextContainer::ContextContainer( Ref< IContext > const& context )
		: m_context( context )
	{
	}

	ContextContainer::ContextContainer( Ref< IContext >&& context )
		: m_context( std::move( context ) )
	{
	}

	ContextContainer::~ContextContainer()
	{
	}

	DataBuffer* ContextContainer::getdefstring()
	{
		StringBuilder sb = m_context->getdefstring( 0 );
		Ref< DataBuffer > defstring = sb.combine();
		return defstring.detach();
	}

	ContextContainer* ContextContainer::getsystemcontext()
	{
		return new ContextContainer( exl::getsystemcontext() );
	}

	NodeContainer::NodeContainer( utils::SExpr const* s )
		: m_node( constructfilenode( *s ) )
	{
	}

	NodeContainer::~NodeContainer()
	{
	}

	DataBuffer* NodeContainer::getdefstring()
	{
		StringBuilder sb = m_node->getdefstring( 0 );
		Ref< DataBuffer > defstring = sb.combine();
		return defstring.detach();
	}

	void NodeContainer::build( ContextContainer* context )
	{
		m_node->build( context->m_context );
	}

	ModuleContainer* NodeContainer::compile()
	{
		m_node->compile();
		ILModule& module = m_node->getmodule();
		return new ModuleContainer( m_node, module );
	}

	ModuleContainer::ModuleContainer( IFileNode* node, ILModule& module )
		: m_node( node )
		, m_module( module )
	{
	}

	ModuleContainer::~ModuleContainer()
	{
	}

	utils::SExpr* ModuleContainer::sexport()
	{
		return new utils::SExpr( exportmodule( m_module ) );
	}
}
