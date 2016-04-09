#pragma once

#include <graphics/programtranslator.hpp>
#include <graphics/programtranslator-base.hpp>
#include <graphics/programtranslator-state.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
#include <tuple>
#include <initializer_list>
#include <vector>
#include <map>

namespace graphics
{
	namespace programtranslator
	{
		struct FunctionSignature
		{
			programtoken m_token;
			std::vector< int > m_argtypes;

			FunctionSignature(
				programtoken token, std::initializer_list< int > argtypes )
				: m_token( token )
				, m_argtypes( argtypes )
			{
			}

			bool operator<( FunctionSignature const& other ) const
			{
				return std::tie( m_token, m_argtypes ) <
					std::tie( other.m_token, other.m_argtypes );
			}
		};

		struct FunctionFactoryState
		{
			utils::Ref< Value >( *m_factory )(
				TranslatorState* ts,
				FunctionFactoryState const* fs, utils::Ref< Value > const* args );
			utils::String m_fname;
			int m_rettype;
			std::vector< int > m_argpos;

			FunctionFactoryState(
				utils::Ref< Value >( *factory )(
					TranslatorState* ts,
					FunctionFactoryState const* fs,
					utils::Ref< Value > const* args ),
				utils::String const& fname,
				int rettype,
				std::initializer_list< int > argpos )
				: m_factory( factory )
				, m_fname( fname )
				, m_rettype( rettype )
				, m_argpos( argpos )
			{
			}
		};

		struct Value_Function: Value
		{
			std::vector< utils::Ref< Value > > m_args;
			utils::String m_fname;

			Value_Function(
				TranslatorState* ts,
				FunctionFactoryState const* fs, utils::Ref< Value > const* args );
			virtual utils::Ref< StringBuilder > getstring() override;
			static utils::Ref< Value > factory(
				TranslatorState* ts,
				FunctionFactoryState const* fs, utils::Ref< Value > const* args );
		};

		struct Value_Unary: Value_Function
		{
			Value_Unary(
				TranslatorState* ts,
				FunctionFactoryState const* fs, utils::Ref< Value > const* args );
			virtual utils::Ref< StringBuilder > getstring() override;
			static utils::Ref< Value > factory(
				TranslatorState* ts,
				FunctionFactoryState const* fs, utils::Ref< Value > const* args );
		};

		struct Value_Binary: Value_Function
		{
			Value_Binary(
				TranslatorState* ts,
				FunctionFactoryState const* fs, utils::Ref< Value > const* args );
			virtual utils::Ref< StringBuilder > getstring() override;
			static utils::Ref< Value > factory(
				TranslatorState* ts,
				FunctionFactoryState const* fs, utils::Ref< Value > const* args );
		};

		Value_Function::Value_Function(
			TranslatorState* ts,
			FunctionFactoryState const* fs, utils::Ref< Value > const* args )
			: m_fname( fs->m_fname )
		{
			m_type = fs->m_rettype;
			for( int argpos : fs->m_argpos )
			{
				Value* arg = args[ argpos ];
				m_args.push_back( arg );
				arg->m_usecount += 1;
			}
		}

		utils::Ref< StringBuilder > Value_Function::getstring()
		{
			if( m_args.size() == 0 )
			{
				return StringBuilder::construct(
					m_fname, "()" );
			}
			else
			{
				auto sb = StringBuilder::construct(
					m_fname, "(", m_args[ 0 ]->getstring() );
				auto it = ++m_args.begin();
				auto eit = m_args.end();
				for( ; it != eit; ++it )
				{
					sb = StringBuilder::construct(
						sb, ",", ( *it )->getstring() );
				}
				return StringBuilder::construct(
						sb, ")" );
			}
		}

		utils::Ref< Value > Value_Function::factory(
			TranslatorState* ts,
			FunctionFactoryState const* fs, utils::Ref< Value > const* args )
		{
			return utils::Ref< Value_Function >::create( ts, fs, args );
		}

		Value_Unary::Value_Unary(
			TranslatorState* ts,
			FunctionFactoryState const* fs, utils::Ref< Value > const* args )
			: Value_Function( ts, fs, args )
		{
		}

		utils::Ref< StringBuilder > Value_Unary::getstring()
		{
			return StringBuilder::construct(
				"(", m_fname, m_args[ 0 ]->getstring(), ")" );
		}

		utils::Ref< Value > Value_Unary::factory(
			TranslatorState* ts,
			FunctionFactoryState const* fs, utils::Ref< Value > const* args )
		{
			return utils::Ref< Value_Unary >::create( ts, fs, args );
		}

		Value_Binary::Value_Binary(
			TranslatorState* ts,
			FunctionFactoryState const* fs, utils::Ref< Value > const* args )
			: Value_Function( ts, fs, args )
		{
		}

		utils::Ref< StringBuilder > Value_Binary::getstring()
		{
			return StringBuilder::construct(
				"(", m_args[ 0 ]->getstring(), m_fname,
				m_args[ 1 ]->getstring(), ")" );
		}

		utils::Ref< Value > Value_Binary::factory(
			TranslatorState* ts,
			FunctionFactoryState const* fs, utils::Ref< Value > const* args )
		{
			return utils::Ref< Value_Binary >::create( ts, fs, args );
		}

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

		std::map< FunctionSignature, FunctionFactoryState > FunctionMap =
			CreateFunctionMap();
	}
}
