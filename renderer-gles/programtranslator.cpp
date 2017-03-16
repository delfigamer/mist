#include <renderer-d3d9/programtranslator.hpp>

#include <renderer-state/programtranslator-cpp.hpp>

namespace graphics
{
	namespace programtranslator
	{
		char const* const typestr[] = {
			"float",
			"vec2",
			"vec3",
			"vec4",
			"mat2",
			"mat3",
			"mat4",
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
				valuetype::float1,
				valuetype::float2,
				valuetype::float3,
				valuetype::float4,
			} )
			{
				ret.emplace(
					FunctionSignature( programtoken::multiply, { type, type } ),
					FunctionFactoryState(
						Value_Binary::factory, "*", type, { 0, 1 } ) );
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
						Value_Function::factory, "matrixCompMult", type, { 0, 1 } ) );
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
						Value_Binary::factory, "*", type, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature(
						programtoken::multiply, { type, valuetype::float1 } ),
					FunctionFactoryState(
						Value_Binary::factory, "*", type, { 0, 1 } ) );
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
						Value_Binary::factory, "*", vectype, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { vectype, mattype } ),
					FunctionFactoryState(
						Value_Binary::factory, "*", vectype, { 0, 1 } ) );
				ret.emplace(
					FunctionSignature( programtoken::dot, { mattype, mattype } ),
					FunctionFactoryState(
						Value_Binary::factory, "*", mattype, { 0, 1 } ) );
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
					"texture2D", valuetype::float4, { 0, 1 } ) );
			return ret;
		}

		Ref< StringBuilder > Value_Attribute::getstring(
			Ref< StringBuilder >* defs )
		{
			static String const attrstr[] = {
				"var_a0",
				"var_a1",
				"var_a2",
				"var_a3",
				"var_a4",
				"var_a5",
				"var_a6",
				"var_a7",
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

		String const attributevarstr[ 8 ] = {
			"varying vec4 var_a0;\n",
			"varying vec4 var_a1;\n",
			"varying vec4 var_a2;\n",
			"varying vec4 var_a3;\n",
			"varying vec4 var_a4;\n",
			"varying vec4 var_a5;\n",
			"varying vec4 var_a6;\n",
			"varying vec4 var_a7;\n",
		};
		String const attributeattrstr[ 8 ] = {
			"attribute vec4 attr_a0;\n",
			"attribute vec4 attr_a1;\n",
			"attribute vec4 attr_a2;\n",
			"attribute vec4 attr_a3;\n",
			"attribute vec4 attr_a4;\n",
			"attribute vec4 attr_a5;\n",
			"attribute vec4 attr_a6;\n",
			"attribute vec4 attr_a7;\n",
		};
		String const attributeassignstr[ 8 ] = {
			"var_a0=attr_a0;\n",
			"var_a1=attr_a1;\n",
			"var_a2=attr_a2;\n",
			"var_a3=attr_a3;\n",
			"var_a4=attr_a4;\n",
			"var_a5=attr_a5;\n",
			"var_a6=attr_a6;\n",
			"var_a7=attr_a7;\n",
		};
		String const texturedefstr[ 8 ] = {
			"sampler2D texture_0;\n",
			"sampler2D texture_1;\n",
			"sampler2D texture_2;\n",
			"sampler2D texture_3;\n",
			"sampler2D texture_4;\n",
			"sampler2D texture_5;\n",
			"sampler2D texture_6;\n",
			"sampler2D texture_7;\n",
		};

		Ref< StringBuilder > makeshaders_vsh(
			TranslatorState* ts )
		{
			auto vsh = StringBuilder::construct(
				"// built from a mistsh bytecode\n"
				"precision highp float;\n"
				"uniform mat4 worldviewmatrix;\n" );
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh = StringBuilder::construct(
						vsh,
						attributevarstr[ i ] );
				}
			}
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					vsh = StringBuilder::construct(
						vsh,
						attributeattrstr[ i ] );
				}
			}
			vsh = StringBuilder::construct(
				vsh,
				"void main(){\n" );
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
				"gl_Position = worldviewmatrix*attr_a0;\n"
				"}\n" );
			return vsh;
		}

		Ref< StringBuilder > makeshaders_fsh(
			TranslatorState* ts,
			Ref< Value > const& value )
		{
			auto fsh = StringBuilder::construct(
				"// built from a mistsh bytecode\n"
				"precision highp float;\n" );
			for( int i = 0; i < 8; ++i )
			{
				if( ts->m_attributeused[ i ] )
				{
					fsh = StringBuilder::construct(
						fsh,
						attributevarstr[ i ] );
				}
			}
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
				"void main(){\n" );
			Ref< StringBuilder > vdefs;
			auto vstr = value->getstring( &vdefs );
			fsh = StringBuilder::construct(
				fsh,
				vdefs,
				"gl_FragColor = ",
				vstr,
				";\n"
				"}\n" );
			return fsh;
		}
	}
}
