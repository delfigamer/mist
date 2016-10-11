#include <renderer-d3d9/programtranslator.hpp>
#include <renderer-d3d9/common.hpp>

#include <renderer-state/programtranslator-cpp.hpp>

namespace graphics
{
	namespace programtranslator
	{
		String const typestr[] = {
			"float",
			"float2",
			"float3",
			"float4",
			"float2x2",
			"float3x3",
			"float4x4",
			"sampler2D",
		};

		std::map< FunctionSignature, FunctionFactoryState > CreateFunctionMap()
		{
			std::map< FunctionSignature, FunctionFactoryState > ret;
			for( auto type : {
				valuetype::float1,
				valuetype::float2,
				valuetype::float3,
				valuetype::float4,
				valuetype::float2x2,
				valuetype::float3x3,
				valuetype::float4x4,
			} )
			{
				for( auto i : {
					std::make_tuple(
						programtoken::add, Value_Binary::factory, "+" ),
					std::make_tuple(
						programtoken::subtract, Value_Binary::factory, "-" ),
					std::make_tuple(
						programtoken::multiply, Value_Binary::factory, "*" ),
					std::make_tuple(
						programtoken::divide, Value_Binary::factory, "/" ),
					std::make_tuple(
						programtoken::modulus, Value_Binary::factory, "%" ),
					std::make_tuple(
						programtoken::power, Value_Function::factory, "pow" ),
				} )
				{
					auto token = std::get< 0 >( i );
					auto factory = std::get< 1 >( i );
					auto fname = std::get< 2 >( i );
					ret.emplace(
						FunctionSignature( token, { type, type } ),
						FunctionFactoryState( factory, fname, type, { 0, 1 } ) );
				}
			}
			for( auto type : {
				valuetype::float2,
				valuetype::float3,
				valuetype::float4,
				valuetype::float2x2,
				valuetype::float3x3,
				valuetype::float4x4,
			} )
			{
				ret.emplace(
					FunctionSignature(
						programtoken::multiply, { valuetype::float1, type } ),
					FunctionFactoryState(
						Value_Function::factory, "mul", type, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature(
						programtoken::multiply, { type, valuetype::float1 } ),
					FunctionFactoryState(
						Value_Function::factory, "mul", type, { 0, 1 } ) );
			}
			for( auto i : {
				std::make_tuple( valuetype::float2, valuetype::float2x2 ),
				std::make_tuple( valuetype::float3, valuetype::float3x3 ),
				std::make_tuple( valuetype::float4, valuetype::float4x4 ),
			} )
			{
				auto vectype = std::get< 0 >( i );
				auto mattype = std::get< 1 >( i );
				ret.emplace(
					FunctionSignature( programtoken::dot, { vectype, vectype } ),
					FunctionFactoryState(
						Value_Function::factory,
						"dot", valuetype::float1, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { mattype, vectype } ),
					FunctionFactoryState(
						Value_Function::factory, "mul", vectype, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { vectype, mattype } ),
					FunctionFactoryState(
						Value_Function::factory, "mul", vectype, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { mattype, mattype } ),
					FunctionFactoryState(
						Value_Function::factory, "mul", mattype, { 0, 1 } ) );
			}
			ret.emplace(
				FunctionSignature(
					programtoken::cross, { valuetype::float3, valuetype::float3 } ),
				FunctionFactoryState(
					Value_Function::factory,
					"cross", valuetype::float3, { 0, 1 } ) );
			ret.emplace(
				FunctionSignature(
					programtoken::sample,
					{ valuetype::sampler2D, valuetype::float2 } ),
				FunctionFactoryState(
					Value_Function::factory,
					"tex2D", valuetype::float4, { 0, 1 } ) );
			return ret;
		}

		Ref< StringBuilder > Value_Attribute::getstring(
			Ref< StringBuilder >* defs )
		{
			static String const attrstr[] = {
				"attrs.a0",
				"attrs.a1",
				"attrs.a2",
				"attrs.a3",
				"attrs.a4",
				"attrs.a5",
				"attrs.a6",
				"attrs.a7",
			};
			return StringBuilder::construct( attrstr[ m_index ] );
		}

		Ref< StringBuilder > Value_Texture::getstring(
			Ref< StringBuilder >* defs )
		{
			static String const texstr[] = {
				"texture_0",
				"texture_1",
				"texture_2",
				"texture_3",
				"texture_4",
				"texture_5",
				"texture_6",
				"texture_7",
			};
			return StringBuilder::construct( texstr[ m_index ] );
		}

		String const attributevarstructstr[ 8 ] = {
			"float4 a0:TEXCOORD0;\n",
			"float4 a1:TEXCOORD1;\n",
			"float4 a2:TEXCOORD2;\n",
			"float4 a3:TEXCOORD3;\n",
			"float4 a4:TEXCOORD4;\n",
			"float4 a5:TEXCOORD5;\n",
			"float4 a6:TEXCOORD6;\n",
			"float4 a7:TEXCOORD7;\n",
		};
		String const attributeattrstructstr[ 8 ] = {
			"float4 a0:TEXCOORD0;\n",
			"float4 a1:TEXCOORD1;\n",
			"float4 a2:TEXCOORD2;\n",
			"float4 a3:TEXCOORD3;\n",
			"float4 a4:TEXCOORD4;\n",
			"float4 a5:TEXCOORD5;\n",
			"float4 a6:TEXCOORD6;\n",
			"float4 a7:TEXCOORD7;\n",
		};
		String const attributeassignstr[ 8 ] = {
			"vars.a0=attrs.a0;\n",
			"vars.a1=attrs.a1;\n",
			"vars.a2=attrs.a2;\n",
			"vars.a3=attrs.a3;\n",
			"vars.a4=attrs.a4;\n",
			"vars.a5=attrs.a5;\n",
			"vars.a6=attrs.a6;\n",
			"vars.a7=attrs.a7;\n",
		};
		String const texturedefstr[ 8 ] = {
			"sampler2D texture_0:register(s0);\n",
			"sampler2D texture_1:register(s1);\n",
			"sampler2D texture_2:register(s2);\n",
			"sampler2D texture_3:register(s3);\n",
			"sampler2D texture_4:register(s4);\n",
			"sampler2D texture_5:register(s5);\n",
			"sampler2D texture_6:register(s6);\n",
			"sampler2D texture_7:register(s7);\n",
		};

		Ref< StringBuilder > makeshaders_vsh(
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

		Ref< StringBuilder > makeshaders_fsh(
			TranslatorState* ts,
			Ref< Value > const& value )
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
			Ref< StringBuilder > vdefs;
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
	}
}
