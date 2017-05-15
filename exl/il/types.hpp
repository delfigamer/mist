#pragma once

#include <exl/types.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <list>
#include <vector>
#include <memory>

namespace exl
{
	enum struct ModuleType
	{
		unit,
	};

	enum struct TokenType
	{
		assign = 0x100,
		add = 0x110,
		subtract = 0x111,
		join = 0x120,
		invoke = 0x170,
		phi = 0x180,
		dummy = 0x181,
		breakpoint = 0x200,
	};

	enum struct ConstType
	{
		string,
		function,
	};

	enum struct ValueType
	{
		nil = 0,
		number = 1,
		bfalse = 2,
		btrue = 3,
		constant = 13,
		variable = 14,
		temporary = 15,
		string0 = 16,
		string1 = 17,
		string2 = 18,
		string3 = 19,
		string4 = 20,
		string5 = 21,
		string6 = 22,
		string7 = 23,
		string8 = 24,
		string9 = 25,
		string10 = 26,
		string11 = 27,
		string12 = 28,
		string13 = 29,
		string14 = 30,
		string15 = 31,
		string16 = 32,
		string17 = 33,
		string18 = 34,
		string19 = 35,
		string20 = 36,
	};

	struct ILToken;
	struct ILBlock;
	struct ILModule;

	struct ILConst
	{
		ConstType type;
		uint32_t depth;
		uint32_t index;
		bool live;

		virtual ~ILConst();
	};

	struct ILValue
	{
		ValueType type;
		union
		{
			struct
			{
				uint32_t depth;
				uint32_t index;
				uint32_t version;
				void* definer;
			};
			double number;
			uint8_t chars[ 20 ];
		};

		void setnil();
		void setnumber( double anumber );
		void setboolean( bool avalue );
		void setconstant( ILConst* adefiner );
		void setconstant( uint32_t adepth, uint32_t aindex );
		void setvariable( uint32_t adepth, uint32_t aindex );
		void settemporary(
			uint32_t adepth, uint32_t aindex, ILToken* adefiner );
		void setstring( unsigned alength, void const* achars );
		void setstring();
	};
	// { ValueType::constant, 0 } -> runtime support
	// { ValueType::constant, 1, 1, 0 } -> filename (even if short)
	// { ValueType::variable, 0, 1 } -> external modules

	struct ILToken
	{
		TokenType type;
		ILBlock* block;
		std::vector< ILValue > inputs;
		std::vector< ILValue > outputs;
		std::vector< ILValue > reads;
		std::vector< ILValue > writes;
		bool live;
	};

	struct ILBranch
	{
		ILValue condition;
		ILBlock* target;
	};

	struct ILBlock
	{
		std::list< ILToken > tokens;
		std::vector< ILBranch > branches;
		bool livebranch;

		ILToken* appendtoken();
		ILBranch* appendbranch();
	};

	struct ILSymbolDef
	{
		uint32_t depth;
		uint32_t index;
		name_t fullname;
		Ref< DataBuffer > typeinfo;
		DefPos defpos;
	};

	struct ILBody
	{
		std::vector< std::unique_ptr< ILConst > > constants;
		std::list< ILBlock > blocks;
		std::vector< ILSymbolDef > symbols;
		ILModule* module;
		ILBlock* entryblock;
		ILBlock* exitblock;
		ILBlock* currentblock;
		ILBody* owner;
		uint32_t depth;
		uint32_t lasttemp;

		void reset( ILBody* owner );
		void newtemp( ILValue& value );
		template< typename T >
		T* appendconstant();
		ILBlock* appendblock();
		ILToken* appendtoken();
		void appendassignment( ILValue const& target, ILValue const& source );
		void appendassignment( ILValue const& target );
		ILSymbolDef* appendsymbol();
	};
	// fixed constants
	// [ 0 ] -> filename

	struct ILModule
	{
		ModuleType type;
		ILBody body;
	};

	struct ILStringConst: ILConst
	{
		Ref< DataBuffer > string;
	};

	struct ILFunctionConst: ILConst
	{
		std::vector< uint32_t > args;
		std::vector< ILValue > results;
		ILBody body;
	};

	template< typename T >
	T* ILBody::appendconstant()
	{
		T* ptr;
		std::unique_ptr< T > uptr( ptr = new T );
		uptr->depth = depth;
		uptr->index = constants.size() + 1;
		constants.push_back( std::move( uptr ) );
		return ptr;
	}
}
