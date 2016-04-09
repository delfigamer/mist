#pragma once

#include <graphics/programtranslator.hpp>
#include <utils/string.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
#include <cstring>

namespace graphics
{
	namespace programtranslator
	{
		struct StringBuilder: utils::RefObject
		{
			utils::Ref< StringBuilder > m_prefix;
			utils::String m_center;
			utils::Ref< StringBuilder > m_suffix;

			StringBuilder( utils::Ref< StringBuilder > prefix,
				utils::String center, utils::Ref< StringBuilder > suffix );
			size_t getlength();
			size_t write( uint8_t* buffer, size_t bufsize );
			template< typename F, typename S, typename... T >
			static utils::Ref< StringBuilder > construct(
				F&& first, S&& sedond, T&&... args );
			static utils::Ref< StringBuilder > construct( utils::String str );
			static utils::Ref< StringBuilder > construct( StringBuilder* sb );
		};

		StringBuilder::StringBuilder( utils::Ref< StringBuilder > prefix,
			utils::String center, utils::Ref< StringBuilder > suffix )
			: m_prefix( prefix )
			, m_center( center )
			, m_suffix( suffix )
		{
		}

		size_t StringBuilder::getlength()
		{
			size_t length = m_center.getlength();
			if( m_prefix )
			{
				length += m_prefix->getlength();
			}
			if( m_suffix )
			{
				length += m_suffix->getlength();
			}
			return length;
		}

		size_t StringBuilder::write( uint8_t* buffer, size_t bufsize )
		{
			size_t length = 0;
			if( m_prefix && bufsize > 0 )
			{
				size_t res = m_prefix->write( buffer, bufsize );
				length += res;
				buffer += res;
				bufsize -= res;
			}
			size_t clen = m_center.getlength();
			if( clen > bufsize )
			{
				clen = bufsize;
			}
			if( clen > 0 )
			{
				memcpy( buffer, m_center.getchars(), clen );
				length += clen;
				buffer += clen;
				bufsize -= clen;
			}
			if( m_suffix && bufsize > 0 )
			{
				size_t res = m_suffix->write( buffer, bufsize );
				length += res;
			}
			return length;
		}

		template< typename F, typename S, typename... T >
		utils::Ref< StringBuilder > StringBuilder::construct(
			F&& first, S&& second, T&&... args )
		{
			auto left = construct( std::forward< F >( first ) );
			auto right = construct(
				std::forward< S >( second ), std::forward< T >( args )... );
			return utils::Ref< StringBuilder >::create(
				left, nullptr, right );
		}

		utils::Ref< StringBuilder > StringBuilder::construct( utils::String str )
		{
			return utils::Ref< StringBuilder >::create(
				nullptr, str, nullptr );
		}

		utils::Ref< StringBuilder > StringBuilder::construct( StringBuilder* sb )
		{
			return utils::Ref< StringBuilder >::create(
				nullptr, nullptr, sb );
		}

		namespace valuetype
		{
			enum
			{
				float1,
				float2,
				float3,
				float4,
				float2x2,
				float3x3,
				float4x4,
				sampler2D,
			};
		}

		struct Value: utils::RefObject
		{
			int m_type;
			int m_usecount = 0;

			virtual utils::Ref< StringBuilder > getstring() = 0;
		};
	}
}
