#pragma once

#include <graphics/programtranslator.hpp>
#include <graphics/programtranslator-base.hpp>
#include <graphics/programtranslator-state.hpp>
#include <graphics/programtranslator-generators.hpp>
#include <graphics/programtranslator-functions.hpp>
#include <map>

namespace graphics
{
	namespace programtranslator
	{
		void step_generator( TranslatorState* ts, programtoken token )
		{
			int it = int( token );
			if( it >= int( programtoken::attribute_0 ) &&
				it <= int( programtoken::attribute_7 ) )
			{
				int index = it - int( programtoken::attribute_0 );
				ts->push( utils::Ref< Value_Attribute >::create( ts, index ) );
			}
			else if( it >= int( programtoken::texture_0 ) &&
				it <= int( programtoken::texture_7 ) )
			{
				int index = it - int( programtoken::texture_0 );
				ts->push( utils::Ref< Value_Texture >::create( ts, index ) );
			}
			else
			{
				ts->error( "invalid token" );
			}
		}

		void step_unary( TranslatorState* ts, programtoken token )
		{
			utils::Ref< Value > args[ 1 ];
			args[ 0 ] = ts->pop();
			auto it = FunctionMap.find(
				{ token, { args[ 0 ]->m_type } } );
			if( it == FunctionMap.end() )
			{
				ts->error( "invalid function" );
			}
			else
			{
				utils::Ref< Value > value = it->second.m_factory(
					ts, &it->second, args );
				ts->push( value );
			}
		}

		void step_binary( TranslatorState* ts, programtoken token )
		{
			utils::Ref< Value > args[ 2 ];
			args[ 1 ] = ts->pop();
			args[ 0 ] = ts->pop();
			auto it = FunctionMap.find(
				{ token, { args[ 0 ]->m_type, args[ 1 ]->m_type } } );
			if( it == FunctionMap.end() )
			{
				ts->error( "invalid function" );
			}
			else
			{
				utils::Ref< Value > value = it->second.m_factory(
					ts, &it->second, args );
				ts->push( value );
			}
		}

		void step_literal( TranslatorState* ts, programtoken token )
		{
			float elems[ 16 ];
			int size = ( ( ( int( token ) >> 4 ) & 15 ) + 1 );
			for( int i = 0; i < size; ++i )
			{
				elems[ i ] = ts->nextfloat();
			}
			static std::map< programtoken, int > typemap = {
				{ programtoken::literal_float1, valuetype::float1 },
				{ programtoken::literal_float2, valuetype::float2 },
				{ programtoken::literal_float3, valuetype::float3 },
				{ programtoken::literal_float4, valuetype::float4 },
				{ programtoken::literal_float2x2, valuetype::float2x2 },
				{ programtoken::literal_float3x3, valuetype::float3x3 },
				{ programtoken::literal_float4x4, valuetype::float4x4 },
			};
			auto it = typemap.find( token );
			if( it == typemap.end() )
			{
				ts->error( "invalid token" );
			}
			else
			{
				ts->push( utils::Ref< Value_Literal >::create(
					it->second, elems, elems + size ) );
			}
		}

		void step( TranslatorState* ts )
		{
			programtoken token = ts->nexttoken();
			switch( int( token ) >> 8 )
			{
			case 0: // generators
				step_generator( ts, token );
			break;
			case 1: case 2: // swizzles
			break;
			case 3: // unary functions
			break;
			case 4: // binary functions
				step_binary( ts, token );
			break;
			case 5: // ternary functions
			break;
			case 6: // literals
				step_literal( ts, token );
			break;
			case 7: // constructors
			break;
			case 8: // special operations
			break;
			default:
				ts->error( "invalid token" );
			};
		}
	}
}
