#ifndef GRAPHICS_MESHDATA_HPP__
#define GRAPHICS_MESHDATA_HPP__ 1

#include "meshbuffer.hpp"
#include "resource.hpp"
#include <utils/ref.hpp>
#include <atomic>

namespace graphics
{
	class MeshData: public Resource
	{
	public:
		struct Vertex
		{
			float pos_x;
			float pos_y;
			float pos_z;
			float tex1_x;
			float tex1_y;
			float tex2_x;
			float tex2_y;
			uint32_t color;
		};

	private:
		IDirect3DVertexBuffer9* m_vertexbuffer;
		IDirect3DIndexBuffer9* m_indexbuffer;
		IDirect3DVertexDeclaration9* m_vertexdeclaration;
		int m_vertexbuffersize;
		int m_indexbuffersize;
		int m_vertexcount;
		int m_indexcount;
		std::atomic< MeshBuffer* > m_frontbuffer;
		std::atomic< MeshBuffer* > m_backbuffer;
		std::atomic< bool > m_dirty;
		MeshBuffer m_buffer1;
		MeshBuffer m_buffer2;

	protected:
		virtual void doadvance( IDirect3DDevice9* device, int framecount ) override;

	public:
		MeshData();
		virtual ~MeshData() override;
		MeshData( MeshData const& ) = delete;
		MeshData( MeshData&& ) = delete;
		MeshData& operator=( MeshData const& ) = delete;
		MeshData& operator=( MeshData&& ) = delete;

		bool bind( IDirect3DDevice9* device, int* vertexcount, int* indexcount );
		MeshBuffer* trylockfront();
		void unlockfront( MeshBuffer* mb );
		MeshBuffer* trylockback();
		void unlockback( MeshBuffer* mb );
	};

	extern "C"
	{
		MeshData* graphics_meshdata_new() noexcept;
		int graphics_meshdata_trylock( MeshData* md, MeshBuffer** pmb ) noexcept;
		bool graphics_meshdata_unlock( MeshData* md, MeshBuffer* mb ) noexcept;
	}
}

#endif
