#pragma once

#include <renderer-state/programtranslator.hpp>
#include <common/string.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <cstring>

namespace graphics
{
	namespace programtranslator
	{
		struct StringBuilder: RefObject
		{
			Ref< StringBuilder > m_prefix;
			String m_center;
			Ref< StringBuilder > m_suffix;

			StringBuilder( Ref< StringBuilder > prefix,
				String center, Ref< StringBuilder > suffix );
			size_t getlength();
			size_t write( uint8_t* buffer, size_t bufsize );
			template< typename F, typename S, typename... T >
			static Ref< StringBuilder > construct(
				F&& first, S&& sedond, T&&... args );
			static Ref< StringBuilder > construct( String str );
			static Ref< StringBuilder > construct( StringBuilder* sb );
			static Ref< StringBuilder > construct();
		};

		StringBuilder::StringBuilder( Ref< StringBuilder > prefix,
			String center, Ref< StringBuilder > suffix )
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
		Ref< StringBuilder > StringBuilder::construct(
			F&& first, S&& second, T&&... args )
		{
			auto left = construct( std::forward< F >( first ) );
			auto right = construct(
				std::forward< S >( second ), std::forward< T >( args )... );
			return Ref< StringBuilder >::create(
				left, nullptr, right );
		}

		Ref< StringBuilder > StringBuilder::construct( String str )
		{
			return Ref< StringBuilder >::create(
				nullptr, str, nullptr );
		}

		Ref< StringBuilder > StringBuilder::construct( StringBuilder* sb )
		{
			return Ref< StringBuilder >::create(
				nullptr, nullptr, sb );
		}

		Ref< StringBuilder > StringBuilder::construct()
		{
			return Ref< StringBuilder >::create(
				nullptr, nullptr, nullptr );
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

		// back-end-dependent
		extern String const typestr[];

		struct Value: RefObject
		{
			int m_type;
			int m_usecount = 0;

			virtual Ref< StringBuilder > getstring(
				Ref< StringBuilder >* defs ) = 0;
		};
	}
}
