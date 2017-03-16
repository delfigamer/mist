#pragma once

#include <common/stringbuilder.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <vector>
#include <cinttypes>

namespace exl
{
	typedef std::vector< uint8_t > identifier_t;

	struct IObject;
	struct IContext;
	struct INode;
	struct IBlock;
	struct IStatement;
	struct IExpression;

	struct IObject
	{
		virtual ptrdiff_t AddRef() = 0;
		virtual ptrdiff_t Release() = 0;
		virtual Ref< StringBuilder > getdefstring( size_t depth ) = 0;
	};

	struct IContext: virtual IObject
	{
	};

	struct INode: virtual IObject
	{
		virtual void gettextrange(
			int* sr, int* sc, int* er, int* ec, Ref< DataBuffer >* filename ) = 0;
	};

	struct IBlock: virtual INode
	{
	};

	struct IStatement: virtual INode
	{
	};

	struct IExpression: virtual INode
	{
	};
}
