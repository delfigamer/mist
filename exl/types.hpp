#pragma once

#include <common/stringbuilder.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <map>
#include <list>
#include <vector>
#include <memory>

namespace exl
{
	typedef std::vector< uint8_t > identifier_t;
	typedef std::vector< identifier_t > name_t;

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
		invoke = 0x130,

		breakpointnote = -0x100,
		symbolnote = -0x101,
	};

	enum struct ValueType
	{
		reg,
		number,
		string,
		function,
	};

	enum struct SymbolType
	{
		symconst,
		symlocal,
		symargument,
	};

	struct IObject;
	struct IContext;
	struct ITypeInfo;
	struct INode;
	struct IFileNode;
	struct IBlock;
	struct IStatement;
	struct IExpression;
	struct IValue;
	struct IInvocation;
	struct IConstValue;
	struct INumberValue;
	struct IStringValue;
	struct ITypeCast;
	struct ISymbol;

	struct TextPos
	{
		int row = 0;
		int col = 0;
	};

	struct TextRange
	{
		TextPos spos;
		TextPos epos;
		Ref< DataBuffer > filename;
	};

	struct DefPos
	{
		TextPos pos;
		Ref< DataBuffer > filename;
	};

	struct FullType
	{
		Ref< ITypeInfo > type;
		bool readable;
		bool writable;
	};

	struct ArgDef
	{
		DefPos defpos;
		bool named;
		identifier_t name;
		FullType fulltype;
		Ref< IValue > initvalue;
	};

	struct ILBlock;

	struct ILValue
	{
		virtual ~ILValue() = default;
		ValueType type;
	};

	struct ILToken
	{
		virtual ~ILToken() = default;
		TokenType type;
	};

	struct ILBranch
	{
		std::unique_ptr< ILValue > condition;
		ILBlock* target;
	};

	struct ILBlock
	{
		std::list< std::unique_ptr< ILToken > > tokens;
		std::vector< ILBranch > branches;
	};

	struct ILBody
	{
		std::list< std::unique_ptr< ILBlock > > blocks;
		std::vector< std::unique_ptr< ILToken > > notes;
		ILBlock* entryblock;
		ILBlock* exitblock;
		ILBlock* currentblock;
		uint32_t depth;
		uint32_t stackpos;
	};

	struct ILModule
	{
		ModuleType type;
		ILBody body;
	};

	struct ILRegisterValue: ILValue
	{
		uint64_t reg;
	};

	struct ILNumberValue: ILValue
	{
		double number;
	};

	struct ILStringValue: ILValue
	{
		Ref< DataBuffer > string;
	};

	struct ILFunctionValue: ILValue
	{
		std::vector< uint64_t > args;
		ILBody body;
	};

	struct ILOperator: ILToken
	{
		std::vector< std::unique_ptr< ILValue > > inputs;
		std::vector< std::unique_ptr< ILValue > > outputs;
	};

	struct ILBreakpointNote: ILToken
	{
		TextPos pos;
		Ref< DataBuffer > filename;
	};

	struct ILSymbolNote: ILToken
	{
		uint64_t reg;
		SymbolType symboltype;
		name_t fullname;
		Ref< DataBuffer > typeinfo;
		DefPos defpos;
	};

	struct IObject
	{
		virtual ptrdiff_t AddRef() = 0;
		virtual ptrdiff_t Release() = 0;
		virtual StringBuilder getdefstring( size_t depth ) = 0;
	};

	struct IContext: virtual IObject
	{
		virtual name_t const& getnameprefix() = 0;
		virtual Ref< ISymbol > getsymbol( identifier_t const& name ) = 0;
		// on success, returns nullptr
		// on failure, returns the existing symbol
		virtual Ref< ISymbol > trysetsymbol(
			identifier_t const& name, ISymbol* symbol ) = 0;
	};

	struct ITypeInfo: virtual IObject
	{
		// implicit cast from source->gettype() to this
		virtual Ref< ITypeCast > cast( IValue* source ) = 0;
		virtual Ref< IInvocation > invoke(
			IValue* target,
			std::vector< Ref< IValue > > const& args ) = 0;
	};

	struct INode: virtual IObject
	{
		virtual TextRange gettextrange() = 0;
		virtual void build( IContext* context ) = 0;
	};

	struct IFileNode: virtual INode
	{
		virtual ILModule& compile() = 0;
	};

	// compile_reserve: reserve resources, e.g. registers for symbols
	struct IBlock: virtual INode
	{
		virtual void compilereserve( ILBody* body ) = 0;
		virtual void compileemit( ILBody* body ) = 0;
	};

	struct IStatement: virtual INode
	{
		virtual void compilereserve( ILBody* body ) = 0;
		virtual void compileemit( ILBody* body ) = 0;
	};

	struct IExpression: virtual INode
	{
		virtual Ref< IValue > getvalue() = 0;
	};

	struct IValue: virtual IObject
	{
		virtual FullType getfulltype() = 0;
		virtual Ref< IConstValue > getconstvalue() = 0;
		virtual uint64_t compileread( ILBody* body ) = 0;
		virtual void compilewrite( ILBody* body, uint64_t value ) = 0;
	};

	struct IInvocation: virtual IValue
	{
	};

	struct IConstValue: virtual IValue
	{
		virtual Ref< ITypeInfo > gettypevalue() = 0;
		virtual Ref< INumberValue > getnumbervalue() = 0;
		virtual Ref< IStringValue > getstringvalue() = 0;
	};

	struct INumberValue: virtual IConstValue
	{
		virtual double getnumbercontent() = 0;
	};

	struct IStringValue: virtual IConstValue
	{
		virtual Ref< DataBuffer > getstringcontent() = 0;
	};

	struct ITypeCast: virtual IValue
	{
	};

	struct ISymbol: virtual IObject
	{
		virtual DefPos getdefpos() = 0;
		virtual FullType getfulltype() = 0;
		virtual Ref< IConstValue > getconstvalue() = 0;
		virtual void setregister( uint64_t reg ) = 0;
		virtual uint64_t compileread( ILBody* body, uint64_t base ) = 0;
		virtual void compilewrite(
			ILBody* body, uint64_t base, uint64_t value ) = 0;
	};
}
