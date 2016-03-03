#ifndef GRAPHICS_RESOURCE_HPP__
#define GRAPHICS_RESOURCE_HPP__ 1

#include <utils/refobject.hpp>
#include <common.hpp>
#include <atomic>
#include <d3d9.h>

namespace graphics
{
	R_CLASS( name = resource )
	class Resource: public utils::RefObject
	{
	protected:
		std::atomic< int > m_lastframe;

	protected:
		virtual void doadvance() = 0;

	public:
		Resource();
		virtual ~Resource() override;
		virtual void destroy() override;
		Resource( Resource const& ) = delete;
		Resource& operator=( Resource const& ) = delete;

		void advance();
	};
}

#endif
