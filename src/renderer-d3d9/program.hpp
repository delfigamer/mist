#pragma once

#include <renderer-d3d9/resource.hpp>
#include <common/ref.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>
#include <d3dx9.h>
#include <atomic>

namespace graphics
{
	R_CLASS( name = program )
	class Program: public Resource
	{
	private:
		bool m_cache;
		AtomicRef< DataBuffer > m_source;
		Ref< DataBuffer > m_binary;
		Ref< DataBuffer > m_log;
		std::atomic< bool > m_ready;
		bool m_valid;
		Ref< IDirect3DVertexShader9 > m_vertexshader;
		Ref< IDirect3DPixelShader9 > m_fragmentshader;

	protected:
		virtual void doadvance() override;

	public:
		// Program( DataBuffer* binary, bool cache );
		Program( DataBuffer* source, bool cache, int );
		~Program();
		Program( Program const& ) = delete;
		Program& operator=( Program const& ) = delete;

		bool bind();

		// R_METHOD() static Program* load( DataBuffer* binary, bool cache )
		// {
			// return new Program( binary, cache );
		// }

		R_METHOD() static Program* translate( DataBuffer* source, bool cache )
		{
			return new Program( source, cache, 0 );
		}

		R_METHOD() bool isready()
		{
			return m_ready.load( std::memory_order_acquire );
		}

		R_METHOD() bool isvalid()
		{
			return m_valid;
		}

		R_METHOD() DataBuffer* getbinary()
		{
			::addref( m_binary );
			return m_binary;
		}

		R_METHOD() DataBuffer* getlog()
		{
			::addref( m_log );
			return m_log;
		}

		R_METHOD() void clearcache();
	};
}
