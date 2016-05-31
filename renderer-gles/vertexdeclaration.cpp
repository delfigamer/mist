#include <renderer-gles/vertexbuffer.hpp>
#include <renderer-gles/context.hpp>
#include <renderer-gles/common.hpp>
#include <atomic>

#include <cstdio>

namespace graphics
{
	namespace
	{
		void validatedecldata( size_t length, void* data )
		{
			bool attrused[ Limits::AttributeCount ] = { 0 };
			VertexDeclElement* elems = ( VertexDeclElement* )data;
			for( size_t i = 0; i < length / sizeof( VertexDeclElement ); ++i )
			{
				if( elems[ i ].attribute < 0 ||
					elems[ i ].attribute >= Limits::AttributeCount )
				{
					throw utils::StrException(
						"invalid attribute index %i for element %i",
						elems[ i ].attribute, i );
				}
				if( elems[ i ].format < 0 ||
					elems[ i ].format >=
						int( vertexelementformat::invalid ) )
				{
					throw utils::StrException(
						"invalid format %i for element %i",
						elems[ i ].format, i );
				}
				if( attrused[ elems[ i ].attribute ] )
				{
					throw utils::StrException(
						"duplicate attribute index %i at element %i",
						elems[ i ].attribute, i );
				}
				attrused[ elems[ i ].attribute ] = true;
			}
		}

		struct elemformat_t
		{
			GLint size;
			GLenum type;
			GLboolean normalized;
		};

		elemformat_t const elemformattable[] = {
			{ 1, GL_FLOAT, false },
			{ 2, GL_FLOAT, false },
			{ 3, GL_FLOAT, false },
			{ 4, GL_FLOAT, false },
			{ 4, GL_UNSIGNED_BYTE, false },
			{ 2, GL_SHORT, false },
			{ 4, GL_SHORT, false },
			{ 4, GL_UNSIGNED_BYTE, true },
			{ 2, GL_SHORT, true },
			{ 4, GL_SHORT, true },
			{ 2, GL_UNSIGNED_SHORT, true },
			{ 4, GL_UNSIGNED_SHORT, true },
		};
	}

	void VertexDeclaration::doadvance()
	{
		if( m_attribcount == -1 )
		{
			utils::DataBuffer* data = m_data;
			std::atomic_thread_fence( std::memory_order_acquire );
			VertexDeclElement* elems = ( VertexDeclElement* )data->m_data;
			size_t attribcount = data->m_length / sizeof( VertexDeclElement );
			for( size_t i = 0; i < attribcount; ++i )
			{
				m_layout[ i ].index = elems[ i ].attribute;
				elemformat_t const& format = elemformattable[ elems[ i ].format ];
				m_layout[ i ].size = format.size;
				m_layout[ i ].type = format.type;
				m_layout[ i ].normalized = format.normalized;
				m_layout[ i ].offset = ( void* )size_t( elems[ i ].offset );
			}
			m_attribcount = attribcount;
			m_data = nullptr;
		}
	}

	VertexDeclaration::VertexDeclaration(
		utils::DataBuffer* data, size_t vertexsize )
		: m_attribcount( -1 )
		, m_vertexsize( vertexsize )
	{
		validatedecldata( data->m_length, data->m_data );
		std::atomic_thread_fence( std::memory_order_release );
		m_data = data;
	}

	VertexDeclaration::~VertexDeclaration()
	{
	}

	bool VertexDeclaration::bind( size_t* vertexsize )
	{
		if( m_attribcount == -1 )
		{
			return false;
		}
		if( Context::CurrentVertexDeclaration != this )
		{
			for( int i = 0; i < m_attribcount; ++i )
			{
				glEnableVertexAttribArray( m_layout[ i ].index );
				checkerror();
				glVertexAttribPointer(
					m_layout[ i ].index,
					m_layout[ i ].size,
					m_layout[ i ].type,
					m_layout[ i ].normalized,
					m_vertexsize,
					m_layout[ i ].offset );
				checkerror();
			}
			Context::CurrentVertexDeclaration = this;
		}
		*vertexsize = m_vertexsize;
		return true;
	}
}
