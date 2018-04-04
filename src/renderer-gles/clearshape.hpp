#pragma once

#include <renderer-gles/shape.hpp>
#include <common/flaglock.hpp>
#include <common.hpp>
#include <atomic>
#include <mutex>

namespace graphics
{
	R_CLASS( name = clearshape )
	class ClearShape: public Shape
	{
	private:
		typedef FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		struct color_t
		{
			mutex_t m_mutex;
			float m_data[ 4 ];

			void get( float* data );
			void set( float const* data );
		};

	private:
		std::atomic< int > m_flags;
		color_t m_color;
		std::atomic< float > m_depth;
		std::atomic< uint32_t > m_stencil;

	protected:
		virtual void doadvance() override;

	public:
		ClearShape( bool ccolor, bool cdepth, bool cstencil );
		~ClearShape();

		virtual void paint() override;

		R_METHOD() static ClearShape* create(
			bool ccolor, bool cdepth, bool cstencil )
		{
			return new ClearShape( ccolor, cdepth, cstencil );
		}
		R_METHOD() void setcolor( float const* value );
		R_METHOD() void setdepth( float value );
		R_METHOD() void setstencil( uint32_t value );
	};
}
