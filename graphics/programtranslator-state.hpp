#pragma once

#include <graphics/programtranslator.hpp>
#include <graphics/programtranslator-base.hpp>
#include <utils/strexception.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
#include <vector>
#include <stdexcept>

namespace graphics
{
	namespace programtranslator
	{
		struct TranslatorState
		{
			utils::Ref< utils::DataBuffer > m_input;
			int m_position = 0;
			std::vector< utils::Ref< Value > > m_stack;
			int m_lastid = 0;
			bool m_attributeused[ 8 ];
			bool m_textureused[ 8 ];

			void error( char const* msg );
			bool iseof();
			int const* nextptr();
			programtoken nexttoken();
			float nextfloat();
			utils::Ref< Value > const& peek( int depth );
			utils::Ref< Value > pop();
			void push( utils::Ref< Value > const& value );
			int generateid();
		};

		void TranslatorState::error( char const* msg )
		{
			throw utils::StrException( "%s at %#x", msg, m_position );
		}

		bool TranslatorState::iseof()
		{
			return ( m_position * 4 + 4 ) > m_input->m_length;
		}

		int const* TranslatorState::nextptr()
		{
			m_position += 1;
			if( m_position * 4 > m_input->m_length )
			{
				throw std::runtime_error( "unexpected end of stream" );
			}
			return ( int const* )( m_input->m_data ) + ( m_position - 1 );
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

		utils::Ref< Value > const& TranslatorState::peek( int depth )
		{
			if( depth >= int( m_stack.size() ) )
			{
				error( "invalid stack index" );
			}
			return m_stack[ m_stack.size() - depth - 1 ];
		}

		utils::Ref< Value > TranslatorState::pop()
		{
			if( m_stack.empty() )
			{
				error( "stack is empty" );
			}
			utils::Ref< Value > value = std::move( m_stack.back() );
			m_stack.pop_back();
			return value;
		}

		void TranslatorState::push( utils::Ref< Value > const& value )
		{
			m_stack.push_back( value );
		}

		int TranslatorState::generateid()
		{
			return ++m_lastid;
		}
	}
}

