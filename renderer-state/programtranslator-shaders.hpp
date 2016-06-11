#pragma once

#include <renderer-d3d9/programtranslator.hpp>
#include <renderer-d3d9/programtranslator-base.hpp>
#include <renderer-d3d9/programtranslator-state.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>

namespace graphics
{
	namespace programtranslator
	{
		utils::String const attributevarstructstr[ 8 ] = {
			"float4 a0:TEXCOORD0;\n",
			"float4 a1:TEXCOORD1;\n",
			"float4 a2:TEXCOORD2;\n",
			"float4 a3:TEXCOORD3;\n",
			"float4 a4:TEXCOORD4;\n",
			"float4 a5:TEXCOORD5;\n",
			"float4 a6:TEXCOORD6;\n",
			"float4 a7:TEXCOORD7;\n",
		};
		utils::String const attributeattrstructstr[ 8 ] =  {
			"float4 a0:TEXCOORD0;\n",
			"float4 a1:TEXCOORD1;\n",
			"float4 a2:TEXCOORD2;\n",
			"float4 a3:TEXCOORD3;\n",
			"float4 a4:TEXCOORD4;\n",
			"float4 a5:TEXCOORD5;\n",
			"float4 a6:TEXCOORD6;\n",
			"float4 a7:TEXCOORD7;\n",
		};
		utils::String const attributeassignstr[ 8 ] = {
			"vars.a0=attrs.a0;\n",
			"vars.a1=attrs.a1;\n",
			"vars.a2=attrs.a2;\n",
			"vars.a3=attrs.a3;\n",
			"vars.a4=attrs.a4;\n",
			"vars.a5=attrs.a5;\n",
			"vars.a6=attrs.a6;\n",
			"vars.a7=attrs.a7;\n",
		};
		utils::String const texturedefstr[ 8 ] = {
			"sampler2D texture_0:register(s0);\n",
			"sampler2D texture_1:register(s1);\n",
			"sampler2D texture_2:register(s2);\n",
			"sampler2D texture_3:register(s3);\n",
			"sampler2D texture_4:register(s4);\n",
			"sampler2D texture_5:register(s5);\n",
			"sampler2D texture_6:register(s6);\n",
			"sampler2D texture_7:register(s7);\n",
		};

		utils::Ref< StringBuilder > makeshaders_vsh(
			TranslatorState* ts )
		{
			auto vsh = StringBuilder::construct(
				"// built from a mistsh bytecode\n"
				"float4x4 worldviewmatrix:register(c0);\n"
				"struct varyings_t{\n" );
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh = StringBuilder::construct(
						vsh,
						attributevarstructstr[ i ] );
				}
			}
			vsh = StringBuilder::construct(
				vsh,
				"};\n"
				"struct attributes_t{\n" );
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh = StringBuilder::construct(
						vsh,
						attributeattrstructstr[ i ] );
				}
			}
			vsh = StringBuilder::construct(
				vsh,
				"};\n"
				"float4 main("
					"in attributes_t attrs,out varyings_t vars):POSITION{\n" );
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh = StringBuilder::construct(
						vsh,
						attributeassignstr[ i ] );
				}
			}
			vsh = StringBuilder::construct(
				vsh,
				"return mul(worldviewmatrix,attrs.a0);\n"
				"}\n" );
			return vsh;
		}

		utils::Ref< StringBuilder > makeshaders_fsh(
			TranslatorState* ts,
			utils::Ref< Value > const& value )
		{
			auto fsh = StringBuilder::construct(
				"// built from a mistsh bytecode\n"
				"struct varyings_t{\n" );
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					fsh = StringBuilder::construct(
						fsh,
						attributevarstructstr[ i ] );
				}
			}
			fsh = StringBuilder::construct(
				fsh,
				"};\n" );
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_textureused[ i ] )
				{
					fsh = StringBuilder::construct(
						fsh,
						texturedefstr[ i ] );
				}
			}
			fsh = StringBuilder::construct(
				fsh,
				"float4 main(in varyings_t attrs):COLOR0{\n" );
			utils::Ref< StringBuilder > vdefs;
			auto vstr = value->getstring( &vdefs );
			fsh = StringBuilder::construct(
				fsh,
				vdefs,
				"return ",
				vstr,
				";\n"
				"}\n" );
			return fsh;
		}

		void makeshaders(
			TranslatorState* ts,
			utils::Ref< StringBuilder >* pvsh,
			utils::Ref< StringBuilder >* pfsh )
		{
			utils::Ref< programtranslator::Value > value = ts->pop();
			*pvsh = makeshaders_vsh( ts );
			*pfsh = makeshaders_fsh( ts, value );
		}
	}
}

