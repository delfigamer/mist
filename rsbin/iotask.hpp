#pragma once

#include <common/string.hpp>
#include <common/refobject.hpp>
#include <common.hpp>
#include <atomic>

namespace rsbin
{
	R_CLASS( name = iotask )
	class IoTask: public RefObject
	{
	public:
		size_t m_result;
		String m_error;
		std::atomic< bool > m_finished;

		IoTask()
			: m_result( 0 )
			, m_finished( false )
		{
		}
		virtual ~IoTask();
		IoTask( IoTask const& ) = delete;
		IoTask& operator=( IoTask const& ) = delete;

		virtual bool iterate();
		R_METHOD() bool isfinished() NOEXCEPT
		{
			return m_finished.load( std::memory_order_acquire );
		}
		R_METHOD() size_t getresult() NOEXCEPT { return m_result; }
		R_METHOD( stringwrap ) char const* geterror() NOEXCEPT
		{
			return m_error ? m_error.getchars() : 0;
		}
	};
}
