#include <renderer-d3d9/programtranslator.hpp>
#include <renderer-d3d9/common.hpp>

#include <renderer-state/programtranslator-cpp.hpp>

namespace graphics
{
	namespace programtranslator
	{
		Ref< DataBuffer > const typestr[] = {
			"float"_db,
			"float2"_db,
			"float3"_db,
			"float4"_db,
			"float2x2"_db,
			"float3x3"_db,
			"float4x4"_db,
			"sampler2D"_db,
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
						programtoken::add, Value_Binary::factory, "+"_db ),
					std::make_tuple(
						programtoken::subtract, Value_Binary::factory, "-"_db ),
					std::make_tuple(
						programtoken::multiply, Value_Binary::factory, "*"_db ),
					std::make_tuple(
						programtoken::divide, Value_Binary::factory, "/"_db ),
					std::make_tuple(
						programtoken::modulus, Value_Binary::factory, "%"_db ),
					std::make_tuple(
						programtoken::power, Value_Function::factory, "pow"_db ),
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
						Value_Function::factory, "mul"_db, type, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature(
						programtoken::multiply, { type, valuetype::float1 } ),
					FunctionFactoryState(
						Value_Function::factory, "mul"_db, type, { 0, 1 } ) );
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
						"dot"_db, valuetype::float1, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { mattype, vectype } ),
					FunctionFactoryState(
						Value_Function::factory, "mul"_db, vectype, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { vectype, mattype } ),
					FunctionFactoryState(
						Value_Function::factory, "mul"_db, vectype, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { mattype, mattype } ),
					FunctionFactoryState(
						Value_Function::factory, "mul"_db, mattype, { 0, 1 } ) );
			}
			ret.emplace(
				FunctionSignature(
					programtoken::cross, { valuetype::float3, valuetype::float3 } ),
				FunctionFactoryState(
					Value_Function::factory,
					"cross"_db, valuetype::float3, { 0, 1 } ) );
			ret.emplace(
				FunctionSignature(
					programtoken::sample,
					{ valuetype::sampler2D, valuetype::float2 } ),
				FunctionFactoryState(
					Value_Function::factory,
					"tex2D"_db, valuetype::float4, { 0, 1 } ) );
			return ret;
		}

		StringBuilder Value_Attribute::getstring( StringBuilder* defs )
		{
			static Ref< DataBuffer > const attrstr[] = {
				"attrs.a0"_db,
				"attrs.a1"_db,
				"attrs.a2"_db,
				"attrs.a3"_db,
				"attrs.a4"_db,
				"attrs.a5"_db,
				"attrs.a6"_db,
				"attrs.a7"_db,
			};
			return attrstr[ m_index ];
		}

		StringBuilder Value_Texture::getstring( StringBuilder* defs )
		{
			static Ref< DataBuffer > const texstr[] = {
				"texture_0"_db,
				"texture_1"_db,
				"texture_2"_db,
				"texture_3"_db,
				"texture_4"_db,
				"texture_5"_db,
				"texture_6"_db,
				"texture_7"_db,
			};
			return texstr[ m_index ];
		}

		Ref< DataBuffer > const attributevarstructstr[ 8 ] = {
			"float4 a0:TEXCOORD0;\n"_db,
			"float4 a1:TEXCOORD1;\n"_db,
			"float4 a2:TEXCOORD2;\n"_db,
			"float4 a3:TEXCOORD3;\n"_db,
			"float4 a4:TEXCOORD4;\n"_db,
			"float4 a5:TEXCOORD5;\n"_db,
			"float4 a6:TEXCOORD6;\n"_db,
			"float4 a7:TEXCOORD7;\n"_db,
		};
		Ref< DataBuffer > const attributeattrstructstr[ 8 ] = {
			"float4 a0:TEXCOORD0;\n"_db,
			"float4 a1:TEXCOORD1;\n"_db,
			"float4 a2:TEXCOORD2;\n"_db,
			"float4 a3:TEXCOORD3;\n"_db,
			"float4 a4:TEXCOORD4;\n"_db,
			"float4 a5:TEXCOORD5;\n"_db,
			"float4 a6:TEXCOORD6;\n"_db,
			"float4 a7:TEXCOORD7;\n"_db,
		};
		Ref< DataBuffer > const attributeassignstr[ 8 ] = {
			"vars.a0=attrs.a0;\n"_db,
			"vars.a1=attrs.a1;\n"_db,
			"vars.a2=attrs.a2;\n"_db,
			"vars.a3=attrs.a3;\n"_db,
			"vars.a4=attrs.a4;\n"_db,
			"vars.a5=attrs.a5;\n"_db,
			"vars.a6=attrs.a6;\n"_db,
			"vars.a7=attrs.a7;\n"_db,
		};
		Ref< DataBuffer > const texturedefstr[ 8 ] = {
			"sampler2D texture_0:register(s0);\n"_db,
			"sampler2D texture_1:register(s1);\n"_db,
			"sampler2D texture_2:register(s2);\n"_db,
			"sampler2D texture_3:register(s3);\n"_db,
			"sampler2D texture_4:register(s4);\n"_db,
			"sampler2D texture_5:register(s5);\n"_db,
			"sampler2D texture_6:register(s6);\n"_db,
			"sampler2D texture_7:register(s7);\n"_db,
		};

		StringBuilder makeshaders_vsh(
			TranslatorState* ts )
		{
			StringBuilder vsh;
			vsh <<
				"// built from a mistsh bytecode\n"_db
				"float4x4 worldviewmatrix:register(c0);\n"_db
				"struct varyings_t{\n"_db;
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh << attributevarstructstr[ i ];
				}
			}
			vsh <<
				"};\n"_db
				"struct attributes_t{\n"_db;
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh << attributeattrstructstr[ i ];
				}
			}
			vsh <<
				"};\n"_db
				"float4 main("_db
					"in attributes_t attrs,out varyings_t vars):POSITION{\n"_db;
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh << attributeassignstr[ i ];
				}
			}
			vsh <<
				"return mul(worldviewmatrix,attrs.a0);\n"_db
				"}\n"_db;
			return vsh;
		}

		StringBuilder makeshaders_fsh(
			TranslatorState* ts,
			Ref< Value > const& value )
		{
			StringBuilder fsh;
			fsh <<
				"// built from a mistsh bytecode\n"_db
				"struct varyings_t{\n"_db;
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					fsh << attributevarstructstr[ i ];
				}
			}
			fsh << "};\n"_db;
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_textureused[ i ] )
				{
					fsh << texturedefstr[ i ];
				}
			}
			fsh << "float4 main(in varyings_t attrs):COLOR0{\n"_db;
			StringBuilder vdefs;
			StringBuilder vstr = value->getstring( &vdefs );
			fsh
				<< vdefs
				<< "return "_db << vstr << ";\n"_db
				"}\n"_db;
			return fsh;
		}
	}
}
