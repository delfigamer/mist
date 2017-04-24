#pragma once

#include <exl/object.hpp>
#include <exl/types.hpp>
#include <common/stringbuilder.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <map>

namespace exl
{
	class Context: public Object, public virtual IContext
	{
	private:
		name_t m_nameprefix;
		Ref< IContext > m_parent;
		std::map< identifier_t, Ref< ISymbol > > m_symbols;

	public:
		Context();
		Context( name_t const& nameprefix, IContext* parent = nullptr );
		Context( name_t&& nameprefix, IContext* parent = nullptr );
		~Context();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual name_t const& getnameprefix() override;
		virtual Ref< ISymbol > getsymbol( identifier_t const& name ) override;
		virtual Ref< ISymbol > trysetsymbol(
			identifier_t const& name, ISymbol* symbol ) override;
	};
}
