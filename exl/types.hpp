#pragma once

#include <common/stringbuilder.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	typedef std::vector< uint8_t > identifier_t;
	typedef std::vector< identifier_t > name_t;

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

	struct ILModule;
	struct ILBody;
	struct ILValue;

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
		virtual void compile() = 0;
		virtual ILModule& getmodule() = 0;
	};

	struct IBlock: virtual INode
	{
		virtual void compile( ILBody* body ) = 0;
	};

	struct IStatement: virtual INode
	{
		virtual void compile( ILBody* body ) = 0;
	};

	struct IExpression: virtual INode
	{
		virtual Ref< IValue > getvalue() = 0;
	};

	struct IValue: virtual IObject
	{
		virtual FullType getfulltype() = 0;
		virtual Ref< IConstValue > getconstvalue() = 0;
		virtual void compileread( ILBody* body, ILValue& value ) = 0;
		virtual void compilewrite( ILBody* body, ILValue const& value ) = 0;
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
		virtual void setvalue( ILValue const& value ) = 0;
		virtual void compileread(
			ILBody* body, ILValue const& base, ILValue& value ) = 0;
		virtual void compilewrite(
			ILBody* body, ILValue const& base, ILValue const& value ) = 0;
	};
}
