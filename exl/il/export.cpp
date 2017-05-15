#include <exl/il/export.hpp>
#include <exl/func.hpp>
#include <map>

namespace exl
{
	namespace ILHead
	{
		enum
		{
			block = 0x10,
			branch = 0x11,
			symboldef = 0x12,
			body = 0x13,
			defpos = 0x14,
			op_assign = ( int )TokenType::assign,
			op_add = ( int )TokenType::add,
			op_subtract = ( int )TokenType::subtract,
			op_join = ( int )TokenType::join,
			op_invoke = ( int )TokenType::invoke,
			op_phi = ( int )TokenType::phi,
			op_dummy = ( int )TokenType::dummy,
			op_breakpoint = ( int )TokenType::breakpoint,
			value_nil = 0x1400,
			value_number = 0x1401,
			value_false = 0x1402,
			value_true = 0x1403,
			value_constant = 0x1404,
			value_variable = 0x1405,
			value_temporary = 0x1406,
			value_string = 0x1407,
			const_string = 0x1800,
			const_function = 0x1801,
			module_unit = 0x1c00,
			key_const_depth = 0x2000,
			key_const_index = 0x2001,
			key_stringconst_string = 0x2100,
			key_functionconst_args = 0x2200,
			key_functionconst_results = 0x2201,
			key_functionconst_body = 0x2202,
			key_token_inputs = 0x2300,
			key_token_outputs = 0x2301,
			key_branch_condition = 0x2400,
			key_branch_target = 0x2401,
			key_block_tokens = 0x2500,
			key_block_branches = 0x2501,
			key_symboldef_depth = 0x2600,
			key_symboldef_index = 0x2601,
			key_symboldef_fullname = 0x2602,
			key_symboldef_typeinfo = 0x2603,
			key_symboldef_defpos = 0x2604,
			key_body_constants = 0x2700,
			key_body_blocks = 0x2701,
			key_body_symbols = 0x2702,
			key_body_entryblock = 0x2703,
			key_body_exitblock = 0x2704,
			key_body_depth = 0x2705,
			key_module_body = 0x2800,
		};
	}

	Ref< utils::SENameMap > ILNameMap( new utils::SENameMap{
		{ ILHead::block, "block" },
		{ ILHead::branch, "branch" },
		{ ILHead::symboldef, "symboldef" },
		{ ILHead::body, "body" },
		{ ILHead::defpos, "defpos" },
		{ ILHead::op_assign, "op.assign" },
		{ ILHead::op_add, "op.add" },
		{ ILHead::op_subtract, "op.subtract" },
		{ ILHead::op_join, "op.join" },
		{ ILHead::op_invoke, "op.invoke" },
		{ ILHead::op_phi, "op.phi" },
		{ ILHead::op_dummy, "op.dummy" },
		{ ILHead::op_breakpoint, "op.breakpoint" },
		{ ILHead::value_nil, "value.nil" },
		{ ILHead::value_number, "value.number" },
		{ ILHead::value_false, "value.false" },
		{ ILHead::value_true, "value.true" },
		{ ILHead::value_constant, "value.constant" },
		{ ILHead::value_variable, "value.variable" },
		{ ILHead::value_temporary, "value.temporary" },
		{ ILHead::value_string, "value.string" },
		{ ILHead::const_string, "const.string" },
		{ ILHead::const_function, "const.function" },
		{ ILHead::module_unit, "module.unit" },
		{ ILHead::key_const_depth, "key.const.depth" },
		{ ILHead::key_const_index, "key.const.index" },
		{ ILHead::key_stringconst_string, "key.stringconst.string" },
		{ ILHead::key_functionconst_args, "key.functionconst.args" },
		{ ILHead::key_functionconst_results, "key.functionconst.results" },
		{ ILHead::key_functionconst_body, "key.functionconst.body" },
		{ ILHead::key_token_inputs, "key.token.inputs" },
		{ ILHead::key_token_outputs, "key.token.outputs" },
		{ ILHead::key_branch_condition, "key.branch.condition" },
		{ ILHead::key_branch_target, "key.branch.target" },
		{ ILHead::key_block_tokens, "key.block.tokens" },
		{ ILHead::key_block_branches, "key.block.branches" },
		{ ILHead::key_symboldef_depth, "key.symboldef.depth" },
		{ ILHead::key_symboldef_index, "key.symboldef.index" },
		{ ILHead::key_symboldef_fullname, "key.symboldef.fullname" },
		{ ILHead::key_symboldef_typeinfo, "key.symboldef.typeinfo" },
		{ ILHead::key_symboldef_defpos, "key.symboldef.defpos" },
		{ ILHead::key_body_constants, "key.body.constants" },
		{ ILHead::key_body_blocks, "key.body.blocks" },
		{ ILHead::key_body_symbols, "key.body.symbols" },
		{ ILHead::key_body_entryblock, "key.body.entryblock" },
		{ ILHead::key_body_exitblock, "key.body.exitblock" },
		{ ILHead::key_body_depth, "key.body.depth" },
		{ ILHead::key_module_body, "key.module.body" },
	}, 0 );

	Ref< utils::SEContext > ILContext( new utils::SEContext{
	}, 0 );

	namespace
	{
		utils::SExpr exportbody( ILBody const& body );

		template< typename T, typename F >
		utils::SExpr exportlist( int head, T const& list, F func )
		{
			utils::SExpr ret( head );
			ret.list = apply( list, func );
			return ret;
		}

		utils::SExpr exportvalue( ILValue const& value )
		{
			switch( value.type )
			{
			case ValueType::nil:
			{
				return
					utils::SExpr( ILHead::value_nil, {} );
			}
			case ValueType::number:
			{
				return
					utils::SExpr( ILHead::value_number, {
						utils::SExpr::atom( value.number ) } );
			}
			case ValueType::bfalse:
			{
				return
					utils::SExpr( ILHead::value_false, {} );
			}
			case ValueType::btrue:
			{
				return
					utils::SExpr( ILHead::value_true, {} );
			}
			case ValueType::constant:
			{
				return
					utils::SExpr( ILHead::value_constant, {
						utils::SExpr::atom( value.depth ),
						utils::SExpr::atom( value.index ) } );
			}
			case ValueType::variable:
			{
				return
					utils::SExpr( ILHead::value_variable, {
						utils::SExpr::atom( value.depth ),
						utils::SExpr::atom( value.index ) } );
			}
			case ValueType::temporary:
			{
				return
					utils::SExpr( ILHead::value_temporary, {
						utils::SExpr::atom( value.depth ),
						utils::SExpr::atom( value.index ) } );
			}
			case ValueType::string0:
			case ValueType::string1:
			case ValueType::string2:
			case ValueType::string3:
			case ValueType::string4:
			case ValueType::string5:
			case ValueType::string6:
			case ValueType::string7:
			case ValueType::string8:
			case ValueType::string9:
			case ValueType::string10:
			case ValueType::string11:
			case ValueType::string12:
			case ValueType::string13:
			case ValueType::string14:
			case ValueType::string15:
			case ValueType::string16:
			case ValueType::string17:
			case ValueType::string18:
			case ValueType::string19:
			case ValueType::string20:
			{
				int length = ( int )value.type - ( int )ValueType::string0;
				Ref< DataBuffer > buf = DataBuffer::create( length, value.chars );
				return
					utils::SExpr( ILHead::value_string, {
						utils::SExpr::atom( buf ) } );
			}
			}
			ASSERT( false );
			return utils::SExpr();
		}

		utils::SExpr exportconstant( std::unique_ptr< ILConst > const& uptr )
		{
			ILConst const& constant = *uptr;
			switch( constant.type )
			{
			case ConstType::string:
			{
				ILStringConst const& cstr = ( ILStringConst const& )constant;
				return
					utils::SExpr( ILHead::const_string, {
						utils::SExpr( ILHead::key_stringconst_string, {
							utils::SExpr::atom( cstr.string ) } ) } );
			}
			case ConstType::function:
			{
				ILFunctionConst const& cfunc = ( ILFunctionConst const& )constant;
				return
					utils::SExpr( ILHead::const_function, {
						exportlist( ILHead::key_functionconst_args,
							cfunc.args,
							[]( uint32_t arg ) -> utils::SExpr
							{
								return utils::SExpr::atom( arg );
							} ),
						exportlist( ILHead::key_functionconst_results,
							cfunc.results, exportvalue ),
						utils::SExpr( ILHead::key_functionconst_body, {
							exportbody( cfunc.body ) } ) } );
			}
			}
			ASSERT( false );
			return utils::SExpr();
		}

		utils::SExpr exporttoken( ILToken const& token )
		{
			return
				utils::SExpr( ( int )token.type, {
					exportlist( ILHead::key_token_inputs,
						token.inputs, exportvalue ),
					exportlist( ILHead::key_token_outputs,
						token.outputs, exportvalue ) } );
		}

		utils::SExpr exportbranch(
			ILBranch const& branch,
			std::map< ILBlock const*, int > const& indexmap )
		{
			return
				utils::SExpr( ILHead::branch, {
					utils::SExpr( ILHead::key_branch_condition, {
						exportvalue( branch.condition ) } ),
					utils::SExpr( ILHead::key_branch_target, {
						utils::SExpr::atom( indexmap.at( branch.target ) ) } ) } );
		}

		utils::SExpr exportblock(
			ILBlock const& block,
			std::map< ILBlock const*, int > const& indexmap )
		{
			return
				utils::SExpr( ILHead::block, {
					exportlist( ILHead::key_block_tokens,
						block.tokens, exporttoken ),
					exportlist( ILHead::key_block_branches,
						block.branches,
						[ & ]( ILBranch const& branch ) -> utils::SExpr
						{
							return exportbranch( branch, indexmap );
						} ) } );
		}

		utils::SExpr exportflowgraph(
			int head, std::list< ILBlock > const& blocks,
			std::map< ILBlock const*, int > const& indexmap )
		{
			return
				exportlist( head,
					blocks,
					[ & ]( ILBlock const& block ) -> utils::SExpr
					{
						return
							utils::SExpr( 0, {
								utils::SExpr::atom( indexmap.at( &block ) ),
								exportblock( block, indexmap ) } );
					} );
		}

		utils::SExpr exportident( identifier_t const& name )
		{
			return
				utils::SExpr::atom( DataBuffer::create(
					name.size(), name.data() ) );
		}

		utils::SExpr exportdefpos( DefPos defpos )
		{
			return
				utils::SExpr( ILHead::defpos, {
					utils::SExpr::atom( defpos.pos.row ),
					utils::SExpr::atom( defpos.pos.col ),
					utils::SExpr::atom( defpos.filename ) } );
		}

		utils::SExpr exportsymboldef( ILSymbolDef const& symboldef )
		{
			return
				utils::SExpr( ILHead::symboldef, {
					utils::SExpr( ILHead::key_symboldef_depth, {
						utils::SExpr::atom( symboldef.depth ) } ),
					utils::SExpr( ILHead::key_symboldef_index, {
						utils::SExpr::atom( symboldef.index ) } ),
					exportlist( ILHead::key_symboldef_fullname,
						symboldef.fullname, exportident ),
					utils::SExpr( ILHead::key_symboldef_typeinfo, {
						utils::SExpr::atom( symboldef.typeinfo ) } ),
					utils::SExpr( ILHead::key_symboldef_defpos, {
						exportdefpos( symboldef.defpos ) } ) } );
		}

		utils::SExpr exportbody( ILBody const& body )
		{
			std::map< ILBlock const*, int > indexmap;
			int nextindex = 0;
			for( auto& block: body.blocks )
			{
				indexmap.emplace( &block, nextindex );
				nextindex += 1;
			}
			return
				utils::SExpr( ILHead::body, {
					exportlist( ILHead::key_body_constants,
						body.constants, exportconstant ),
					exportflowgraph( ILHead::key_body_blocks,
						body.blocks, indexmap ),
					exportlist( ILHead::key_body_symbols,
						body.symbols, exportsymboldef ),
					utils::SExpr( ILHead::key_body_entryblock, {
						utils::SExpr::atom( indexmap.at( body.entryblock ) ) } ),
					utils::SExpr( ILHead::key_body_exitblock, {
						utils::SExpr::atom( indexmap.at( body.exitblock ) ) } ),
					utils::SExpr( ILHead::key_body_depth, {
						utils::SExpr::atom( body.depth ) } ) } );
		}
	}

	utils::SExpr exportmodule( ILModule const& module )
	{
		switch( module.type )
		{
		case ModuleType::unit:
			return
				utils::SExpr( ILHead::module_unit, {
					utils::SExpr( ILHead::key_module_body, {
						exportbody( module.body ) } ) } );
		}
		ASSERT( false );
		return utils::SExpr();
	}
}
