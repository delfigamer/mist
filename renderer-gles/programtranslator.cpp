#include <renderer-d3d9/programtranslator.hpp>

#include <renderer-state/programtranslator-cpp.hpp>

namespace graphics
{
	namespace programtranslator
	{
		Ref< DataBuffer > const typestr[] = {
			"float"_db,
			"vec2"_db,
			"vec3"_db,
			"vec4"_db,
			"mat2"_db,
			"mat3"_db,
			"mat4"_db,
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
				valuetype::float1,
				valuetype::float2,
				valuetype::float3,
				valuetype::float4,
			} )
			{
				ret.emplace(
					FunctionSignature( programtoken::multiply, { type, type } ),
					FunctionFactoryState(
						Value_Binary::factory, "*"_db, type, { 0, 1 } ) );
			}
			for( auto type : {
				valuetype::float2x2,
				valuetype::float3x3,
				valuetype::float4x4,
			} )
			{
				ret.emplace(
					FunctionSignature( programtoken::multiply, { type, type } ),
					FunctionFactoryState(
						Value_Function::factory,
						"matrixCompMult"_db, type, { 0, 1 } ) );
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
						Value_Binary::factory, "*"_db, type, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature(
						programtoken::multiply, { type, valuetype::float1 } ),
					FunctionFactoryState(
						Value_Binary::factory, "*"_db, type, { 0, 1 } ) );
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
						Value_Binary::factory, "*"_db, vectype, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { vectype, mattype } ),
					FunctionFactoryState(
						Value_Binary::factory, "*"_db, vectype, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { mattype, mattype } ),
					FunctionFactoryState(
						Value_Binary::factory, "*"_db, mattype, { 0, 1 } ) );
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
					"texture2D"_db, valuetype::float4, { 0, 1 } ) );
			return ret;
		}

		StringBuilder Value_Attribute::getstring( StringBuilder* defs )
		{
			static Ref< DataBuffer > const attrstr[] = {
				"var_a0"_db,
				"var_a1"_db,
				"var_a2"_db,
				"var_a3"_db,
				"var_a4"_db,
				"var_a5"_db,
				"var_a6"_db,
				"var_a7"_db,
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

		Ref< DataBuffer > const attributevarstr[ 8 ] = {
			"varying vec4 var_a0;\n"_db,
			"varying vec4 var_a1;\n"_db,
			"varying vec4 var_a2;\n"_db,
			"varying vec4 var_a3;\n"_db,
			"varying vec4 var_a4;\n"_db,
			"varying vec4 var_a5;\n"_db,
			"varying vec4 var_a6;\n"_db,
			"varying vec4 var_a7;\n"_db,
		};
		Ref< DataBuffer > const attributeattrstr[ 8 ] = {
			"attribute vec4 attr_a0;\n"_db,
			"attribute vec4 attr_a1;\n"_db,
			"attribute vec4 attr_a2;\n"_db,
			"attribute vec4 attr_a3;\n"_db,
			"attribute vec4 attr_a4;\n"_db,
			"attribute vec4 attr_a5;\n"_db,
			"attribute vec4 attr_a6;\n"_db,
			"attribute vec4 attr_a7;\n"_db,
		};
		Ref< DataBuffer > const attributeassignstr[ 8 ] = {
			"var_a0=attr_a0;\n"_db,
			"var_a1=attr_a1;\n"_db,
			"var_a2=attr_a2;\n"_db,
			"var_a3=attr_a3;\n"_db,
			"var_a4=attr_a4;\n"_db,
			"var_a5=attr_a5;\n"_db,
			"var_a6=attr_a6;\n"_db,
			"var_a7=attr_a7;\n"_db,
		};
		Ref< DataBuffer > const texturedefstr[ 8 ] = {
			"sampler2D texture_0;\n"_db,
			"sampler2D texture_1;\n"_db,
			"sampler2D texture_2;\n"_db,
			"sampler2D texture_3;\n"_db,
			"sampler2D texture_4;\n"_db,
			"sampler2D texture_5;\n"_db,
			"sampler2D texture_6;\n"_db,
			"sampler2D texture_7;\n"_db,
		};

		StringBuilder makeshaders_vsh(
			TranslatorState* ts )
		{
			StringBuilder vsh =
				"// built from a mistsh bytecode\n"_db
				"precision highp float;\n"_db
				"uniform mat4 worldviewmatrix;\n"_db;
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh += attributevarstr[ i ];
				}
			}
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh += attributeattrstr[ i ];
				}
			}
			vsh +=
				"void main(){\n"_db;
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh += attributeassignstr[ i ];
				}
			}
			vsh +=
				"gl_Position = worldviewmatrix*attr_a0;\n"_db
				"}\n"_db
				"\0"_db;
			return vsh;
		}

		StringBuilder makeshaders_fsh(
			TranslatorState* ts,
			Ref< Value > const& value )
		{
			StringBuilder fsh =
				"// built from a mistsh bytecode\n"_db
				"precision highp float;\n"_db;
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					fsh += attributevarstr[ i ];
				}
			}
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_textureused[ i ] )
				{
					fsh += texturedefstr[ i ];
				}
			}
			fsh +=
				"void main(){\n"_db;
			StringBuilder vdefs;
			StringBuilder vstr = value->getstring( &vdefs );
			fsh +=
				vdefs +
				"gl_FragColor = "_db + vstr + ";\n"_db
				"}\n"_db
				"\0"_db;
			return fsh;
		}
	}
}
