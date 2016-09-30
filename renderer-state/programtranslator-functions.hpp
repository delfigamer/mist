#pragma once

#include <renderer-state/programtranslator.hpp>
#include <renderer-state/programtranslator-base.hpp>
#include <renderer-state/programtranslator-state.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
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
			Ref< Value >( *m_factory )(
				TranslatorState* ts,
				FunctionFactoryState const* fs, Ref< Value > const* args );
			String m_fname;
			int m_rettype;
			std::vector< int > m_argpos;

			FunctionFactoryState(
				Ref< Value >( *factory )(
					TranslatorState* ts,
					FunctionFactoryState const* fs,
					Ref< Value > const* args ),
				String const& fname,
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
			std::vector< Ref< Value > > m_args;
			String m_fname;
			size_t m_id;
			bool m_defined;

			Value_Function(
				TranslatorState* ts,
				FunctionFactoryState const* fs, Ref< Value > const* args );
			virtual Ref< StringBuilder > getexpression(
				Ref< StringBuilder >* defs );
			virtual Ref< StringBuilder > getstring(
				Ref< StringBuilder >* defs ) override;
			static Ref< Value > factory(
				TranslatorState* ts,
				FunctionFactoryState const* fs, Ref< Value > const* args );
		};

		struct Value_Unary: Value_Function
		{
			Value_Unary(
				TranslatorState* ts,
				FunctionFactoryState const* fs, Ref< Value > const* args );
			virtual Ref< StringBuilder > getexpression(
				Ref< StringBuilder >* defs ) override;
			static Ref< Value > factory(
				TranslatorState* ts,
				FunctionFactoryState const* fs, Ref< Value > const* args );
		};

		struct Value_Binary: Value_Function
		{
			Value_Binary(
				TranslatorState* ts,
				FunctionFactoryState const* fs, Ref< Value > const* args );
			virtual Ref< StringBuilder > getexpression(
				Ref< StringBuilder >* defs ) override;
			static Ref< Value > factory(
				TranslatorState* ts,
				FunctionFactoryState const* fs, Ref< Value > const* args );
		};

		Value_Function::Value_Function(
			TranslatorState* ts,
			FunctionFactoryState const* fs, Ref< Value > const* args )
			: m_fname( fs->m_fname )
			, m_id( ts->generateid() )
		{
			m_type = fs->m_rettype;
			for( int argpos : fs->m_argpos )
			{
				Value* arg = args[ argpos ];
				m_args.push_back( arg );
				arg->m_usecount += 1;
			}
		}

		Ref< StringBuilder > Value_Function::getexpression(
			Ref< StringBuilder >* defs )
		{
			if( m_args.size() == 0 )
			{
				return StringBuilder::construct(
					m_fname, "()" );
			}
			else
			{
				auto sb = StringBuilder::construct(
					m_fname, "(", m_args[ 0 ]->getstring( defs ) );
				auto it = ++m_args.begin();
				auto eit = m_args.end();
				for( ; it != eit; ++it )
				{
					sb = StringBuilder::construct(
						sb, ",", ( *it )->getstring( defs ) );
				}
				return StringBuilder::construct(
						sb, ")" );
			}
		}

		String idtostring( size_t id )
		{
			char buf[ 32 ];
			snprintf( buf, sizeof( buf ), "i_%" PRIuPTR, id );
			return String( buf );
		}

		Ref< StringBuilder > Value_Function::getstring(
			Ref< StringBuilder >* defs )
		{
			if( m_usecount > 1 )
			{
				String idstr = idtostring( m_id );
				if( !m_defined )
				{
					auto expr = getexpression( defs );
					*defs = StringBuilder::construct( *defs,
						typestr[ m_type ], " ", idstr,
						"=", expr, ";\n" );
					m_defined = true;
				}
				return StringBuilder::construct( idstr );
			}
			else
			{
				return getexpression( defs );
			}
		}

		Ref< Value > Value_Function::factory(
			TranslatorState* ts,
			FunctionFactoryState const* fs, Ref< Value > const* args )
		{
			return Ref< Value_Function >::create( ts, fs, args );
		}

		Value_Unary::Value_Unary(
			TranslatorState* ts,
			FunctionFactoryState const* fs, Ref< Value > const* args )
			: Value_Function( ts, fs, args )
		{
		}

		Ref< StringBuilder > Value_Unary::getexpression(
			Ref< StringBuilder >* defs )
		{
			return StringBuilder::construct(
				"(", m_fname, m_args[ 0 ]->getstring( defs ), ")" );
		}

		Ref< Value > Value_Unary::factory(
			TranslatorState* ts,
			FunctionFactoryState const* fs, Ref< Value > const* args )
		{
			return Ref< Value_Unary >::create( ts, fs, args );
		}

		Value_Binary::Value_Binary(
			TranslatorState* ts,
			FunctionFactoryState const* fs, Ref< Value > const* args )
			: Value_Function( ts, fs, args )
		{
		}

		Ref< StringBuilder > Value_Binary::getexpression(
			Ref< StringBuilder >* defs )
		{
			return StringBuilder::construct(
				"(", m_args[ 0 ]->getstring( defs ), m_fname,
				m_args[ 1 ]->getstring( defs ), ")" );
		}

		Ref< Value > Value_Binary::factory(
			TranslatorState* ts,
			FunctionFactoryState const* fs, Ref< Value > const* args )
		{
			return Ref< Value_Binary >::create( ts, fs, args );
		}

		// back-end-dependent
		std::map< FunctionSignature, FunctionFactoryState > CreateFunctionMap();

		std::map< FunctionSignature, FunctionFactoryState > FunctionMap =
			CreateFunctionMap();
	}
}
