#include <exl/il/types.hpp>
#include <stdexcept>

namespace exl
{
	ILConst::~ILConst()
	{
	}

	void ILValue::setnil()
	{
		type = ValueType::nil;
	}

	void ILValue::setnumber( double anumber )
	{
		type = ValueType::number;
		number = anumber;
	}

	void ILValue::setboolean( bool avalue )
	{
		type = avalue ? ValueType::btrue : ValueType::bfalse;
	}

	void ILValue::setconstant( ILConst* adefiner )
	{
		type = ValueType::constant;
		depth = adefiner->depth;
		index = adefiner->index;
		version = 0xffffffff;
		definer = adefiner;
	}

	void ILValue::setconstant( uint32_t adepth, uint32_t aindex )
	{
		type = ValueType::constant;
		depth = adepth;
		index = aindex;
		version = 0xffffffff;
		definer = nullptr;
	}

	void ILValue::setvariable( uint32_t adepth, uint32_t aindex )
	{
		type = ValueType::variable;
		depth = adepth;
		index = aindex;
		version = 0xffffffff;
		definer = nullptr;
	}

	void ILValue::settemporary(
		uint32_t adepth, uint32_t aindex, ILToken* adefiner )
	{
		type = ValueType::temporary;
		depth = adepth;
		index = aindex;
		version = 1;
		definer = adefiner;
	}

	void ILValue::setstring( unsigned alength, void const* achars )
	{
		ASSERT( alength <= 20 );
		type = ( ValueType )( ( int )ValueType::string0 + alength );
		memcpy( chars, achars, alength );
	}

	void ILValue::setstring()
	{
		type = ValueType::string0;
	}

	ILToken* ILBlock::appendtoken()
	{
		tokens.emplace_back();
		ILToken* token = &tokens.back();
		token->block = this;
		return token;
	}

	ILBranch* ILBlock::appendbranch()
	{
		branches.emplace_back();
		return &branches.back();
	}

	void ILBody::reset( ILBody* aowner )
	{
		constants.clear();
		blocks.clear();
		symbols.clear();
		blocks.emplace_back();
		exitblock = &blocks.back();
		blocks.emplace_back();
		entryblock = &blocks.back();
		currentblock = entryblock;
		owner = aowner;
		depth = aowner ? aowner->depth + 1 : 1;
		lasttemp = 0;
	}

	void ILBody::newtemp( ILValue& value )
	{
		if( lasttemp == 0xffffffff )
		{
			throw std::runtime_error(
				"maximum temporary count per block reached" );
		}
		lasttemp += 1;
		value.type = ValueType::temporary;
		value.depth = depth;
		value.index = lasttemp;
	}

	ILBlock* ILBody::appendblock()
	{
		blocks.emplace_back();
		ILBlock* block = &blocks.back();
		currentblock = block;
		return block;
	}

	ILToken* ILBody::appendtoken()
	{
		return currentblock->appendtoken();
	}

	void ILBody::appendassignment( ILValue const& target, ILValue const& source )
	{
		ILToken* op = appendtoken();
		op->type = TokenType::assign;
		op->inputs.resize( 1 );
		op->inputs[ 0 ] = source;
		op->outputs.resize( 1 );
		op->outputs[ 0 ] = target;
	}

	void ILBody::appendassignment( ILValue const& target )
	{
		ILToken* op = appendtoken();
		op->type = TokenType::assign;
		op->inputs.resize( 1 );
		op->inputs[ 0 ].setnil();
		op->outputs.resize( 1 );
		op->outputs[ 0 ] = target;
	}

	ILSymbolDef* ILBody::appendsymbol()
	{
		symbols.emplace_back();
		ILSymbolDef* symbol = &symbols.back();
		symbol->depth = depth;
		symbol->index = symbols.size();
		return symbol;
	}
}
