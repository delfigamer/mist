#include <exl/il/translate.hpp>
#include <exl/func.hpp>
#include <map>

namespace exl
{
	namespace ILHead
	{
		enum
		{
			block = 0x10,
			reg = 0x11,
			branch = 0x12,
			op_assign = ( int )TokenType::assign,
			op_add = ( int )TokenType::add,
			op_subtract = ( int )TokenType::subtract,
			op_join = ( int )TokenType::join,
			op_invoke = ( int )TokenType::invoke,
			note_breakpoint = 0x1000 - ( int )TokenType::breakpointnote,
			note_symbol = 0x1000 - ( int )TokenType::symbolnote,
			value_register = 0x1800,
			value_number = 0x1801,
			value_string = 0x1802,
			value_function = 0x1803,
			key_inputs = 0x2000,
			key_outputs = 0x2001,
			key_register = 0x2002,
			key_symboltype = 0x2003,
			key_fullname = 0x2004,
			key_typeinfo = 0x2005,
			key_defpos = 0x2006,
			key_number = 0x2007,
			key_string = 0x2008,
			key_arguments = 0x2009,
			key_body = 0x200a,
			key_flowgraph = 0x200b,
			key_notes = 0x200c,
			module_unit = 0x2800,
		};
	}

	Ref< utils::SENameMap > ILNameMap( new utils::SENameMap{
		{ ILHead::block, "block" },
		{ ILHead::reg, "reg" },
		{ ILHead::branch, "branch" },
		{ ILHead::op_assign, "op.assign" },
		{ ILHead::op_add, "op.add" },
		{ ILHead::op_subtract, "op.subtract" },
		{ ILHead::op_join, "op.join" },
		{ ILHead::op_invoke, "op.invoke" },
		{ ILHead::note_breakpoint, "note.breakpoint" },
		{ ILHead::note_symbol, "note.symbol" },
		{ ILHead::value_register, "value.register" },
		{ ILHead::value_number, "value.number" },
		{ ILHead::value_string, "value.string" },
		{ ILHead::value_function, "value.function" },
		{ ILHead::key_inputs, "key.inputs" },
		{ ILHead::key_outputs, "key.outputs" },
		{ ILHead::key_register, "key.register" },
		{ ILHead::key_symboltype, "key.symboltype" },
		{ ILHead::key_fullname, "key.fullname" },
		{ ILHead::key_typeinfo, "key.typeinfo" },
		{ ILHead::key_defpos, "key.defpos" },
		{ ILHead::key_number, "key.number" },
		{ ILHead::key_string, "key.string" },
		{ ILHead::key_arguments, "key.arguments" },
		{ ILHead::key_body, "key.body" },
		{ ILHead::key_flowgraph, "key.flowgraph" },
		{ ILHead::key_notes, "key.notes" },
		{ ILHead::module_unit, "module.unit" },
	}, 0 );

	Ref< utils::SEContext > ILContext( new utils::SEContext{
	}, 0 );

	namespace
	{
		utils::SExpr translatebody( ILBody const& body );

		template< typename T, typename F >
		utils::SExpr translatelist( int head, T const& list, F func )
		{
			utils::SExpr ret( head );
			ret.list = apply( list, func );
			return ret;
		}

		utils::SExpr translatereg( uint64_t reg )
		{
			return
				utils::SExpr( ILHead::reg, {
					utils::SExpr::atom( ( double )( uint32_t )( reg >> 32 ) ),
					utils::SExpr::atom( ( double )( uint32_t )reg ) } );
		}

		utils::SExpr translatevalue( std::unique_ptr< ILValue > const& value )
		{
			switch( value->type )
			{
			case ValueType::reg:
			{
				ILRegisterValue* rval = ( ILRegisterValue* )value.get();
				return
					utils::SExpr( ILHead::value_register, {
						translatereg( rval->reg ) } );
			}
			case ValueType::number:
			{
				ILNumberValue* nval = ( ILNumberValue* )value.get();
				return
					utils::SExpr( ILHead::value_number, {
						utils::SExpr::atom( nval->number ) } );
			}
			case ValueType::string:
			{
				ILStringValue* sval = ( ILStringValue* )value.get();
				return
					utils::SExpr( ILHead::value_string, {
						utils::SExpr::atom( sval->string ) } );
			}
			case ValueType::function:
			{
				ILFunctionValue* fval = ( ILFunctionValue* )value.get();
				return
					utils::SExpr( ILHead::value_function, {
						translatelist(
							ILHead::key_arguments, fval->args, translatereg ),
						translatebody( fval->body ) } );
			}
			}
			return utils::SExpr();
		}

		utils::SExpr translatesymboltype( SymbolType st )
		{
			static utils::SExpr constword( ILHead::key_symboltype, {
				utils::SExpr::atom( DataBuffer::create( 5, "const" ) ) } );
			static utils::SExpr localword( ILHead::key_symboltype, {
				utils::SExpr::atom( DataBuffer::create( 5, "local" ) ) } );
			static utils::SExpr argumentword( ILHead::key_symboltype, {
				utils::SExpr::atom( DataBuffer::create( 8, "argument" ) ) } );
			switch( st )
			{
			case SymbolType::symconst:
				return constword;
			case SymbolType::symlocal:
				return localword;
			case SymbolType::symargument:
				return argumentword;
			}
			return utils::SExpr();
		}

		utils::SExpr translateident( identifier_t const& name )
		{
			return
				utils::SExpr::atom( DataBuffer::create(
					name.size(), name.data() ) );
		}

		utils::SExpr translatedefpos( DefPos defpos )
		{
			return
				utils::SExpr( ILHead::key_defpos, {
					utils::SExpr::atom( defpos.pos.row ),
					utils::SExpr::atom( defpos.pos.col ),
					utils::SExpr::atom( defpos.filename ) } );
		}

		utils::SExpr translatetoken( std::unique_ptr< ILToken > const& token )
		{
			switch( token->type )
			{
			case TokenType::assign:
			case TokenType::add:
			case TokenType::subtract:
			case TokenType::join:
			case TokenType::invoke:
			{
				ILOperator* op = ( ILOperator* )token.get();
				return
					utils::SExpr( ( int )op->type, {
						translatelist(
							ILHead::key_inputs, op->inputs, translatevalue ),
						translatelist(
							ILHead::key_outputs, op->outputs, translatevalue ) } );
			}
			case TokenType::breakpointnote:
			{
				ILBreakpointNote* note = ( ILBreakpointNote* )token.get();
				return
					utils::SExpr( ILHead::note_breakpoint, {
						translatedefpos( DefPos{ note->pos, note->filename } ) } );
			}
			case TokenType::symbolnote:
			{
				ILSymbolNote* note = ( ILSymbolNote* )token.get();
				if( note->fullname.empty() )
				{
					return
						utils::SExpr( ILHead::note_symbol, {
							utils::SExpr( ILHead::key_register, {
								translatereg( note->reg ) } ),
							translatesymboltype( note->symboltype ),
							utils::SExpr( ILHead::key_typeinfo, {
								utils::SExpr::atom( note->typeinfo ) } ),
							translatedefpos( note->defpos ) } );
				}
				else
				{
					return
						utils::SExpr( ILHead::note_symbol, {
							utils::SExpr( ILHead::key_register, {
								translatereg( note->reg ) } ),
							translatesymboltype( note->symboltype ),
							translatelist(
								ILHead::key_fullname, note->fullname, translateident ),
							utils::SExpr( ILHead::key_typeinfo, {
								utils::SExpr::atom( note->typeinfo ) } ),
							translatedefpos( note->defpos ) } );
				}
			}
			}
			return utils::SExpr();
		}

		utils::SExpr translatebranch(
			ILBranch const& branch, std::map< ILBlock*, int > const& indexmap )
		{
			return
				utils::SExpr( ILHead::branch, {
					branch.condition
						? translatevalue( branch.condition )
						: utils::SExpr(),
					utils::SExpr::atom( indexmap.at( branch.target ) ) } );
		}

		utils::SExpr translateblock(
			ILBlock const& block, std::map< ILBlock*, int > const& indexmap )
		{
			return
				utils::SExpr( ILHead::block, {
					translatelist( 0, block.tokens, translatetoken ),
					translatelist( 0, block.branches,
						[ & ]( ILBranch const& branch ) -> utils::SExpr
						{
							return translatebranch( branch, indexmap );
						}) } );
		}

		utils::SExpr translateflowgraph( ILBody const& body )
		{
			std::map< ILBlock*, int > indexmap = {
				{ body.exitblock, 0 },
				{ body.entryblock, 1 } };
			int nextindex = 2;
			for( auto& uptr: body.blocks )
			{
				ILBlock* block = uptr.get();
				if( block != body.entryblock && block != body.exitblock )
				{
					indexmap.emplace( block, nextindex );
					nextindex += 1;
				}
			}
			return translatelist( ILHead::key_flowgraph, body.blocks,
				[ & ]( std::unique_ptr< ILBlock > const& uptr ) -> utils::SExpr
				{
					ILBlock* block = uptr.get();
					return
						utils::SExpr( 0, {
							utils::SExpr::atom( indexmap.at( block ) ),
							translateblock( *block, indexmap ) } );
				} );
		}

		utils::SExpr translatebody( ILBody const& body )
		{
			return
				utils::SExpr( ILHead::key_body, {
					translateflowgraph( body ),
					translatelist(
						ILHead::key_notes, body.notes, translatetoken ) } );
		}
	}

	utils::SExpr translatemodule( ILModule const& module )
	{
		switch( module.type )
		{
		case ModuleType::unit:
			return
				utils::SExpr( ILHead::module_unit, {
					translatebody( module.body ) } );
		}
		return utils::SExpr();
	}
}
