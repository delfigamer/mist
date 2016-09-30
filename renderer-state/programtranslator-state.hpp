#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-base.hpp>
#include <common/strexception.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <vector>
#include <stdexcept>
#include <cinttypes>

namespace graphics
{
	namespace programtranslator
	{
		struct TranslatorState
		{
			Ref< DataBuffer > m_input;
			size_t m_position = 0;
			std::vector< Ref< Value > > m_stack;
			size_t m_lastid = 0;
			bool m_attributeused[ 8 ];
			bool m_textureused[ 8 ];

			void error( char const* msg );
			bool iseof();
			uint32_t const* nextptr();
			programtoken nexttoken();
			float nextfloat();
			Ref< Value > const& peek( size_t depth );
			Ref< Value > pop();
			void push( Ref< Value > const& value );
			size_t generateid();
		};

		void TranslatorState::error( char const* msg )
		{
			throw StrException( "%s at %#x", msg, m_position );
		}

		bool TranslatorState::iseof()
		{
			return ( m_position * 4 + 4 ) > m_input->m_length;
		}

		uint32_t const* TranslatorState::nextptr()
		{
			m_position += 1;
			if( m_position * 4 > m_input->m_length )
			{
				throw std::runtime_error( "unexpected end of stream" );
			}
			return ( uint32_t const* )( m_input->m_data ) + m_position - 1;
		}

		programtoken TranslatorState::nexttoken()
		{
			programtoken const* ptoken = ( programtoken const* )nextptr();
			return *ptoken;
		}

		float TranslatorState::nextfloat()
		{
			float const* pfloat = ( float const* )nextptr();
			return *pfloat;
		}

		Ref< Value > const& TranslatorState::peek( size_t depth )
		{
			if( depth >= m_stack.size() )
			{
				error( "invalid stack index" );
			}
			return m_stack[ m_stack.size() - depth - 1 ];
		}

		Ref< Value > TranslatorState::pop()
		{
			if( m_stack.empty() )
			{
				error( "stack is empty" );
			}
			Ref< Value > value = std::move( m_stack.back() );
			m_stack.pop_back();
			return value;
		}

		void TranslatorState::push( Ref< Value > const& value )
		{
			m_stack.push_back( value );
		}

		size_t TranslatorState::generateid()
		{
			return ++m_lastid;
		}
	}
}

