#ifndef RSBIN_IOTASK_HPP__
#define RSBIN_IOTASK_HPP__ 1

#include <utils/string.hpp>
#include <utils/refobject.hpp>
#include <common.hpp>
#include <atomic>

namespace rsbin
{
	R_CLASS( name = iotask )
	class IoTask: public utils::RefObject
	{
	public:
		int m_result;
		utils::String m_error;
		std::atomic< bool > m_finished;

		IoTask()
			: m_result( 0 )
			, m_finished( false )
		{
		}
		virtual ~IoTask() = default;
		IoTask( IoTask const& ) = delete;
		IoTask& operator=( IoTask const& ) = delete;

		virtual bool iterate() = 0;
		R_METHOD() void promote();
		R_METHOD() bool isfinished() NOEXCEPT
		{
			return m_finished.load( std::memory_order_acquire );
		}
		R_METHOD() int getresult() NOEXCEPT { return m_result; }
		R_METHOD( stringwrap ) char const* geterror() NOEXCEPT
		{
			return m_error ? m_error.getchars() : 0;
		}
	};
}

#endif
